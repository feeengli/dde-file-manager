/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liqiang<liqianga@uniontech.com>
 *
 * Maintainer: liqiang<liqianga@uniontech.com>
 *             wangchunlin<wangchunlin@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "canvasmodel.h"
#include "canvasmodel_p.h"
#include "filetreater.h"
#include "view/operator/fileoperaterproxy.h"
#include "dfm-base/interfaces/abstractfileinfo.h"
#include "base/schemefactory.h"
#include "dfm-base/utils/fileutils.h"
#include "base/application/application.h"
#include "base/application/settings.h"

#include <QDateTime>
#include <QMimeData>
#include <QStandardPaths>

DFMBASE_USE_NAMESPACE
DSB_D_BEGIN_NAMESPACE

CanvasModelPrivate::CanvasModelPrivate(CanvasModel *qq)
    : QObject (qq)
    , q(qq)
{
    fileTreater.reset(new FileTreater(qq));
}

CanvasModelPrivate::~CanvasModelPrivate()
{

}

void CanvasModelPrivate::doFileDeleted(const QUrl &url)
{
    {
        QMutexLocker lk(&watcherEventMutex);
        QPair<EventType, QVariant> rmFile(kRmFile, QVariant(url));
        QVariant data = QVariant::fromValue(rmFile);
        watcherEvent.enqueue(data);
    }

    metaObject()->invokeMethod(this, QT_STRINGIFY(doWatcherEvent), Qt::QueuedConnection);
}

void CanvasModelPrivate::doFileCreated(const QUrl &url)
{
    {
        QMutexLocker lk(&watcherEventMutex);
        QPair<EventType, QVariant> rmFile(kAddFile, QVariant(url));
        QVariant data = QVariant::fromValue(rmFile);
        watcherEvent.enqueue(data);
    }

    metaObject()->invokeMethod(this, "doWatcherEvent", Qt::QueuedConnection);
}

void CanvasModelPrivate::doFileRename(const QUrl &oldUrl, const QUrl &newUrl)
{
    {
        QMutexLocker lk(&watcherEventMutex);
        QPair<QUrl, QUrl> urls(oldUrl, newUrl);
        QVariant varUrls = QVariant::fromValue(urls);
        QPair<EventType, QVariant> reFile(kReFile, varUrls);
        QVariant data = QVariant::fromValue(reFile);
        watcherEvent.enqueue(data);
    }

    metaObject()->invokeMethod(this, QT_STRINGIFY(doWatcherEvent), Qt::QueuedConnection);
}

void CanvasModelPrivate::doFileUpdated(const QUrl &url)
{
    {
        QMutexLocker lk(&watcherEventMutex);
        QPair<EventType, QVariant> updateFile(kUpdateFile, QVariant(url));
        QVariant data = QVariant::fromValue(updateFile);
        watcherEvent.enqueue(data);
    }

    metaObject()->invokeMethod(this, "doWatcherEvent", Qt::QueuedConnection);
}

void CanvasModelPrivate::doWatcherEvent()
{
    if (processFileEventRuning)
        return;

    processFileEventRuning = true;
    while (checkFileEventQueue()) {
        QVariant event;
        {
            QMutexLocker lk(&watcherEventMutex);
            event = watcherEvent.dequeue();
        }

        if (!event.canConvert<QPair<EventType, QVariant>>()) {
            qInfo() << "data error format:" << event;
            continue;
        }

        QPair<EventType, QVariant> eventData = event.value<QPair<EventType, QVariant>>();
        const EventType eventType = eventData.first;

        if (kAddFile == eventType) {
            const QUrl &url = eventData.second.toUrl();
            fileTreater->insertChild(url);
        } else if (kRmFile == eventType) {
            const QUrl &url = eventData.second.toUrl();
            fileTreater->removeChild(url);
        } else if (kReFile == eventType) {
            const QPair<QUrl, QUrl> urls = eventData.second.value<QPair<QUrl, QUrl>>();
            const QUrl &oldUrl = urls.first;
            const QUrl &newUrl = urls.second;
            fileTreater->renameChild(oldUrl, newUrl);
        } else if (kUpdateFile == eventType) {
            const QUrl &url = eventData.second.toUrl();
            fileTreater->updateChild(url);
        }
    }
    processFileEventRuning = false;
}

void CanvasModelPrivate::onTraversalFinished()
{
    isUpdatedChildren = true;
    fileTreater->onTraversalFinished();
}

bool CanvasModelPrivate::checkFileEventQueue()
{
    QMutexLocker lk(&watcherEventMutex);
    bool isEmptyQueue = watcherEvent.isEmpty();
    return !isEmptyQueue;
}

CanvasModel::CanvasModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d(new CanvasModelPrivate(this))
{

}

QModelIndex CanvasModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row < 0 || column < 0 || d->fileTreater->childrenCount() <= row) {
        return QModelIndex();
    }
    auto fileInfo = d->fileTreater->fileInfo(row);
    if (!fileInfo) {
        return QModelIndex();
    }

    return createIndex(row, column, fileInfo.data());
}

QModelIndex CanvasModel::index(const QString &fileUrl, int column)
{
    if (fileUrl.isEmpty())
        return QModelIndex();

    if (fileUrl == rootUrl().toString())
        return rootIndex();

    auto fileInfo = d->fileTreater->fileInfo(fileUrl);
    return index(fileInfo, column);
}

QModelIndex CanvasModel::index(const DFMLocalFileInfoPointer &fileInfo, int column) const
{
    if (!fileInfo)
        return QModelIndex();

    int row = (0 < d->fileTreater->childrenCount()) ? d->fileTreater->indexOfChild(fileInfo) : 0;
    return createIndex(row, column, const_cast<LocalFileInfo *>(fileInfo.data()));
}

QModelIndexList CanvasModel::indexs() const
{
    QModelIndexList results;
    for (int i = 0; i < rowCount(); i++) {
        QModelIndex childIndex = index(i);
        results << childIndex;
    }
    return results;
}

QModelIndex CanvasModel::parent(const QModelIndex &child) const
{
    if (child.isValid())
        return rootIndex();

    return QModelIndex();
}

int CanvasModel::rowCount(const QModelIndex &parent) const
{
    if (parent == rootIndex())
        return d->fileTreater->childrenCount();

    return 0;
}

int CanvasModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant CanvasModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this) {
        return QVariant();
    }

    auto indexFileInfo = static_cast<LocalFileInfo *>(index.internalPointer());
    if (!indexFileInfo) {
        return QVariant();
    }
    switch (role) {
    case kFileIconRole:
        return indexFileInfo->fileIcon();
    case kFileNameRole:
        return indexFileInfo->fileName();
    case Qt::EditRole:
    case Qt::DisplayRole:
    case kFileDisplayNameRole:
        return indexFileInfo->fileDisplayName();
    case kFilePinyinName:
        return indexFileInfo->fileDisplayPinyinName();
    case kFileLastModifiedRole:
        return indexFileInfo->lastModified().toString();    // todo by file info: lastModifiedDisplayName
    case kFileSizeRole:
        return indexFileInfo->size();   // todo by file info: sizeDisplayName
    case kFileMimeTypeRole:
        return indexFileInfo->fileMimeType().name();    // todo by file info: mimeTypeDisplayName
    case kExtraProperties:
        return indexFileInfo->extraProperties();
    case kFileBaseNameRole:
        return indexFileInfo->baseName();
    case kFileSuffixRole:
        return indexFileInfo->suffix();
    case kFileNameOfRenameRole:
        return "";  // todo by file info: fileNameOfRename
    case kFileBaseNameOfRenameRole:
        return "";  // todo by file info: baseNameOfRename
    case kFileSuffixOfRenameRole:
        return "";  // todo by file info: suffixOfRename
    default:
        return QString();
    }
}

Qt::ItemFlags CanvasModel::flags(const QModelIndex &index) const
{
    if (index == rootIndex())
        return Qt::ItemIsSelectable|Qt::ItemIsEnabled;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (!index.isValid())
        return flags;

    flags |= Qt::ItemIsDragEnabled;
    flags |= Qt::ItemIsDropEnabled; // todo
    flags |= Qt::ItemIsEditable;
    return flags;
}

bool CanvasModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return d->canFetchMoreFlag;
}

void CanvasModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent)

    d->isUpdatedChildren = false;
    if (!d->traversalThread.isNull()) {
        d->traversalThread->disconnect();
        d->traversalThread->stopAndDeleteLater();
    }

    d->traversalThread.reset(new TraversalDirThread(d->rootUrl, QStringList(), d->filters, QDirIterator::NoIteratorFlags));
    if (Q_UNLIKELY(d->traversalThread.isNull())) {
        d->isUpdatedChildren = true;
        return;
    }

    connect(d->traversalThread.data(), &TraversalDirThread::updateChildren, d->fileTreater.data(), &FileTreater::onUpdateChildren);
    connect(d->traversalThread.data(), &TraversalDirThread::finished, d.data(), &CanvasModelPrivate::onTraversalFinished);

    if (d->canFetchMoreFlag) {
        d->canFetchMoreFlag = false;
        d->traversalThread->start();
    }
}

bool CanvasModel::isRefreshed() const
{
    return d->isUpdatedChildren;
}

QModelIndex CanvasModel::setRootUrl(QUrl url)
{
    if (url.isEmpty())
        url = QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    if (Q_UNLIKELY(!url.isValid())) {
        qWarning() << "root url is invalid:" << url;
        return rootIndex();
    }

    d->rootUrl = url;
    if (!d->watcher.isNull()) {
        disconnect(d->watcher.data(), &AbstractFileWatcher::fileDeleted, d.data(), &CanvasModelPrivate::doFileDeleted);
        disconnect(d->watcher.data(), &AbstractFileWatcher::subfileCreated, d.data(), &CanvasModelPrivate::doFileCreated);
        disconnect(d->watcher.data(), &AbstractFileWatcher::fileRename, d.data(), &CanvasModelPrivate::doFileRename);
        disconnect(d->watcher.data(), &AbstractFileWatcher::fileAttributeChanged, d.data(), &CanvasModelPrivate::doFileUpdated);

    }

    d->watcher = WacherFactory::create<AbstractFileWatcher>(d->rootUrl);
    if (Q_LIKELY(!d->watcher.isNull())) {
        connect(d->watcher.data(), &AbstractFileWatcher::fileDeleted, d.data(), &CanvasModelPrivate::doFileDeleted);
        connect(d->watcher.data(), &AbstractFileWatcher::subfileCreated, d.data(), &CanvasModelPrivate::doFileCreated);
        connect(d->watcher.data(), &AbstractFileWatcher::fileRename, d.data(), &CanvasModelPrivate::doFileRename);
        connect(d->watcher.data(), &AbstractFileWatcher::fileAttributeChanged, d.data(), &CanvasModelPrivate::doFileUpdated);
        d->watcher->startWatcher();
    }

    d->filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System;
    d->whetherShowHiddenFile = Application::instance()->genericAttribute(Application::kShowedHiddenFiles).toBool();
    if (d->whetherShowHiddenFile)
        d->filters |= QDir::Hidden;
    else
        d->filters &= ~QDir::Hidden;

    d->canFetchMoreFlag = true;
    return rootIndex();
}

QUrl CanvasModel::rootUrl() const
{
    return d->rootUrl;
}

QUrl CanvasModel::url(const QModelIndex &index) const
{
    if (!index.isValid())
        return d->rootUrl;

    if (auto info = d->fileTreater->fileInfo(index.row())) {
        return info->url();
    }

    return QUrl();
}

DFMLocalFileInfoPointer CanvasModel::fileInfo(const QModelIndex &index) const
{
    if (!index.isValid())
        return nullptr;

    return d->fileTreater->fileInfo(index.row());
}

QList<QUrl> CanvasModel::files() const
{
    return d->fileTreater->files();
}

QMimeData *CanvasModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *data = new QMimeData();
    QList<QUrl> urls;

    for (const QModelIndex &idx : indexes)
        urls << url(idx);

    data->setUrls(urls);
    return data;
}

bool CanvasModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    QList<QUrl> urlList = data->urls();
    if (urlList.isEmpty())
        return false;

    QUrl targetFileUrl;
    if (!parent.isValid()) {
        // drop file to desktop
        targetFileUrl = rootUrl();
        qInfo() << "drop file to desktop" << targetFileUrl << "data" << urlList << action ;
    } else {
        targetFileUrl = url(parent);
        qInfo() << "drop file to " << targetFileUrl << "data:" << urlList << action;
    }

    auto itemInfo = dfmbase::InfoFactory::create<dfmbase::LocalFileInfo>(targetFileUrl);
    if (!itemInfo) {
        qWarning() << "can not get file info" << targetFileUrl;
        return false;
    }

    if (itemInfo->isSymLink()) {
        targetFileUrl = itemInfo->symLinkTarget();
    }

    // todo Compress

    if (dfmbase::FileUtils::isTrashDesktopFile(targetFileUrl)) {
        FileOperaterProxyIns->dropToTrash(urlList);
        return true;
    } else if (dfmbase::FileUtils::isComputerDesktopFile(targetFileUrl)) {
        return true;
    } else if (dfmbase::FileUtils::isDesktopFile(targetFileUrl)) {
        FileOperaterProxyIns->dropToApp(urlList, targetFileUrl.toLocalFile());
        return true;
    }

    switch (action) {
    case Qt::CopyAction:
    case Qt::MoveAction:
        FileOperaterProxyIns->dropFiles(action, targetFileUrl, urlList);
        break;
    case Qt::LinkAction:
        break;
    default:
        return false;
    }

    return true;
}

QStringList CanvasModel::mimeTypes() const
{
    static QStringList types {QLatin1String("text/uri-list")};
    return types;
}

Qt::DropActions CanvasModel::supportedDragActions() const
{
    // todo
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

Qt::DropActions CanvasModel::supportedDropActions() const
{
    // todo
    return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

void CanvasModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    sort();
}

bool CanvasModel::sort()
{
    return d->fileTreater->sort();
}

Qt::SortOrder CanvasModel::sortOrder() const
{
    return d->fileTreater->sortOrder();
}

void CanvasModel::setSortOrder(const Qt::SortOrder &order)
{
    d->fileTreater->setSortOrder(order);
}

int CanvasModel::sortRole() const
{
    return d->fileTreater->sortRole();
}

void CanvasModel::setSortRole(dfmbase::AbstractFileInfo::SortKey role, Qt::SortOrder order)
{
    d->fileTreater->setSortRole(role, order);
}

bool CanvasModel::showHiddenFiles() const
{
    return d->whetherShowHiddenFile;
}

void CanvasModel::setShowHiddenFiles(const bool isShow)
{
    if (d->whetherShowHiddenFile == isShow)
        return;
    d->whetherShowHiddenFile = isShow;
    if (d->whetherShowHiddenFile)
        d->filters |= QDir::Hidden;
    else
        d->filters &= ~QDir::Hidden;
}

QModelIndex CanvasModel::createIndex(int arow, int acolumn, void *adata) const
{
    return QAbstractItemModel::createIndex(arow, acolumn, adata);
}

DSB_D_END_NAMESPACE
