/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuyangming<liuyangming@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
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
#include "tagmanager.h"
#include "taghelper.h"
#include "tagdbushelper.h"
#include "events/tageventcaller.h"
#include "widgets/tagcolorlistwidget.h"

#include "dfm-base/base/schemefactory.h"
#include "services/filemanager/sidebar/sidebar_defines.h"
#include "dfm-base/dfm_global_defines.h"

#include <QMap>
#include <QColor>
#include <QMenu>
#include <QWidgetAction>

DFMGLOBAL_USE_NAMESPACE
DFMBASE_USE_NAMESPACE
DSB_FM_USE_NAMESPACE
DPTAG_USE_NAMESPACE

TagManager::TagManager(QObject *parent)
    : QObject(parent)
{
    dbusHelper = new TagDBusHelper(this);
    initializeConnection();
}

void TagManager::initializeConnection()
{
    connect(dbusHelper, &TagDBusHelper::addNewTags, this, &TagManager::onTagAdded);
    connect(dbusHelper, &TagDBusHelper::tagsDeleted, this, &TagManager::onTagDeleted);
    connect(dbusHelper, &TagDBusHelper::tagColorChanged, this, &TagManager::onTagColorChanged);
    connect(dbusHelper, &TagDBusHelper::tagNameChanged, this, &TagManager::onTagNameChanged);
    connect(dbusHelper, &TagDBusHelper::filesTagged, this, &TagManager::onFilesTagged);
    connect(dbusHelper, &TagDBusHelper::filesUntagged, this, &TagManager::onFilesUntagged);
}

TagManager *TagManager::instance()
{
    static TagManager ins;
    return &ins;
}

QUrl TagManager::rootUrl()
{
    QUrl rootUrl;
    rootUrl.setScheme(scheme());
    rootUrl.setPath("/");
    return rootUrl;
}

TagManager::TagsMap TagManager::getAllTags()
{
    QMap<QString, QVariant> dataMap = { { QString { " " }, QVariant { QList<QString> { QString { " " } } } } };

    QVariant var = dbusHelper->sendDataToDBus(dataMap, TagActionType::kGetAllTags);
    dataMap = var.toMap();

    TagsMap result;
    QMap<QString, QVariant>::const_iterator it = dataMap.begin();

    for (; it != dataMap.end(); ++it)
        result[it.key()] = TagHelper::instance()->qureyColorByColorName(it.value().toString());

    return result;
}

QMap<QString, QString> TagManager::getTagsColorName(const QStringList &tags) const
{
    QMap<QString, QString> result;

    if (!tags.isEmpty()) {
        QMap<QString, QVariant> dataMap {};

        for (const QString &tagName : tags)
            dataMap[tagName] = QVariant { QList<QString> { QString { " " } } };

        dataMap = dbusHelper->sendDataToDBus(dataMap, TagActionType::kGetTagsColor).toMap();

        QMap<QString, QVariant>::const_iterator it = dataMap.begin();
        for (; it != dataMap.end(); ++it)
            result[it.key()] = it.value().toString();
    }

    return result;
}

TagManager::TagsMap TagManager::getTagsColor(const QList<QString> &tags) const
{
    TagsMap result;

    if (!tags.isEmpty()) {
        QMap<QString, QString> dataMap = getTagsColorName(tags);

        QMap<QString, QString>::const_iterator it = dataMap.begin();
        for (; it != dataMap.end(); ++it)
            result[it.key()] = TagHelper::instance()->qureyColorByColorName(it.value());
    }

    return result;
}

QStringList TagManager::getTagsByUrls(const QList<QUrl> &urlList) const
{
    QMap<QString, QVariant> dataMap;

    if (!urlList.isEmpty()) {
        for (const QUrl &url : urlList) {
            const AbstractFileInfoPointer &info = InfoFactory::create<AbstractFileInfo>(url);
            if (info)
                dataMap[info->filePath()] = QVariant { QList<QString> {} };
        }

        if (dataMap.isEmpty())
            return QList<QString> {};

        QVariant var = dbusHelper->sendDataToDBus(dataMap, TagActionType::kGetTagsThroughFile);
        return var.toStringList();
    }

    return QList<QString> {};
}

QString TagManager::getTagColorName(const QString &tag) const
{
    const TagsMap &dataMap = getTagsColor({ tag });
    const QColor &color = dataMap.value(tag);

    if (!color.isValid())
        return QString();

    return TagHelper::instance()->qureyColorNameByColor(color);
}

QString TagManager::getTagIconName(const QString &tag) const
{
    const QMap<QString, QString> &dataMap = getTagsColorName({ tag });
    if (dataMap.contains(tag))
        return TagHelper::instance()->qureyIconNameByColorName(dataMap.value(tag));

    return QString();
}

QString TagManager::getTagIconName(const QColor &color) const
{
    return TagHelper::instance()->qureyIconNameByColor(color);
}

QStringList TagManager::getFilesByTag(const QString &tag)
{
    QStringList list;

    if (!tag.isEmpty()) {
        QMap<QString, QVariant> dataMap = { { tag, QVariant { QList<QString> { QString { " " } } } } };
        QVariant var = dbusHelper->sendDataToDBus(dataMap, TagActionType::kGetFilesThroughTag);
        list = var.toStringList();
    }

    return list;
}

bool TagManager::setTagsForFiles(const QList<QString> &tags, const QList<QUrl> &files)
{
    if (tags.isEmpty()) {
        const QStringList &tags = TagManager::instance()->getTagsByUrls(files);
        return tags.isEmpty() || TagManager::instance()->removeTagsOfFiles(tags, files);
    }

    return TagManager::instance()->addTagsForFiles(tags, files);
}

bool TagManager::addTagsForFiles(const QList<QString> &tags, const QList<QUrl> &files)
{
    if (!tags.isEmpty() && !files.isEmpty()) {
        QMap<QString, QVariant> tagWithColorName {};

        for (const QString &tagName : tags) {
            QString colorName = TagHelper::instance()->getColorNameByTag(tagName);
            tagWithColorName[tagName] = QVariant { QList<QString> { colorName } };
        }

        QVariant checkTagResult { dbusHelper->sendDataToDBus(tagWithColorName, TagActionType::kBeforeMakeFilesTags) };

        if (checkTagResult.toBool()) {
            QMap<QString, QVariant> fileWithTag {};

            for (const QUrl &url : files) {
                const AbstractFileInfoPointer &info = InfoFactory::create<AbstractFileInfo>(url);
                fileWithTag[info->filePath()] = QVariant { tags };
            }

            qInfo() << fileWithTag;
            QVariant tagResult = dbusHelper->sendDataToDBus(fileWithTag, TagActionType::kMakeFilesTags);

            if (!tagResult.toBool())
                qWarning() << "Create tags successfully! But failed to tag files";

            return true;
        }
    }

    return false;
}

bool TagManager::removeTagsOfFiles(const QList<QString> &tags, const QList<QUrl> &files)
{
    bool result { true };

    if (!tags.isEmpty() && !files.isEmpty()) {
        QMap<QString, QVariant> fileWithTag;

        for (const QUrl &url : files) {
            const AbstractFileInfoPointer &info = InfoFactory::create<AbstractFileInfo>(url);
            fileWithTag[info->filePath()] = QVariant(tags);
        }

        QVariant var = dbusHelper->sendDataToDBus(fileWithTag, TagActionType::kRemoveTagsOfFiles);
        result = var.toBool();
    }

    return result;
}

bool TagManager::changeTagColor(const QString &tagName, const QString &newTagColor)
{
    bool result = false;

    if (!tagName.isEmpty() && !newTagColor.isEmpty()) {
        QMap<QString, QVariant> stringVar = { { tagName, QVariant { QList<QString> { newTagColor } } } };
        QVariant var = dbusHelper->sendDataToDBus(stringVar, TagActionType::kChangeTagColor);
        result = var.toBool();
    }

    return result;
}

bool TagManager::changeTagName(const QString &tagName, const QString &newName)
{
    bool result = false;

    if (!tagName.isEmpty() && !newName.isEmpty()) {
        QMap<QString, QVariant> oldAndNewName = { { tagName, QVariant { newName } } };
        QVariant var = dbusHelper->sendDataToDBus(oldAndNewName, TagActionType::kChangeTagName);
        result = var.toBool();
    }

    return result;
}

void TagManager::deleteTags(const QList<QString> &tags)
{
    if (!tags.isEmpty()) {
        QMap<QString, QVariant> tagMap {};

        for (const QString &tagName : tags) {
            tagMap[tagName] = QVariant { QList<QString> {} };
        }

        dbusHelper->sendDataToDBus(tagMap, TagActionType::kDeleteTags);
    }
}

void TagManager::deleteFiles(const QList<QUrl> &urls)
{
    QStringList tagNames;
    QList<QUrl> localFiles;

    for (const QUrl &url : urls) {
        if (!url.fragment(QUrl::FullyDecoded).isEmpty()) {
            localFiles << url;
        } else {
            tagNames.append(TagHelper::instance()->getTagNameFromUrl(url));
        }
    }

    if (!localFiles.isEmpty()) {
        return;
    }

    deleteTags(tagNames);
}

bool TagManager::canTagFile(const AbstractFileInfoPointer &fileInfo) const
{
    QString filePath = fileInfo->filePath();

    if (!filePath.startsWith("/home/") && !filePath.startsWith("/data/home/"))
        return false;

    return true;
}

bool TagManager::paintListTagsHandle(int role, const QUrl &url, QPainter *painter, QRectF *rect)
{
    if (role != kItemFileDisplayNameRole && role != kItemNameRole)
        return false;

    QStringList tags = getTagsByUrls({ url });
    TagsMap tagMap = getTagsColor(tags);
    QList<QColor> colors;

    TagsMap::const_iterator it = tagMap.begin();
    while (it != tagMap.end()) {
        colors.append(it.value());
        ++it;
    }

    if (!colors.isEmpty()) {
        QRectF boundingRect(0, 0, (colors.size() + 1) * kTagDiameter / 2, kTagDiameter);
        boundingRect.moveCenter(rect->center());
        boundingRect.moveRight(rect->right());

        TagHelper::instance()->paintTags(painter, boundingRect, colors);

        rect->setRight(boundingRect.left() - 10);
    }

    return false;
}

bool TagManager::paintIconTagsHandle(int role, const QUrl &url, QPainter *painter, QRectF *rect)
{
    if (role != kItemFileDisplayNameRole && role != kItemNameRole)
        return false;

    QStringList tags = getTagsByUrls({ url });
    TagsMap tagMap = getTagsColor(tags);
    QList<QColor> colors;

    TagsMap::const_iterator it = tagMap.begin();
    while (it != tagMap.end()) {
        colors.append(it.value());
        ++it;
    }

    if (!colors.isEmpty()) {
        QRectF boundingRect(0, 0, (colors.size() + 1) * kTagDiameter / 2, kTagDiameter);
        boundingRect.moveCenter(rect->center());
        boundingRect.moveTop(rect->top());

        TagHelper::instance()->paintTags(painter, boundingRect, colors);

        rect->setTop(boundingRect.bottom());
    }

    return false;
}

void TagManager::contenxtMenuHandle(quint64 windowId, const QUrl &url, const QPoint &globalPos)
{
    QMenu *menu = new QMenu;

    // base action
    menu->addAction(QObject::tr("Open in new window"), [url]() {
        TagEventCaller::sendOpenWindow(url);
    });

    auto newTabAct = menu->addAction(QObject::tr("Open in new tab"), [windowId, url]() {
        TagEventCaller::sendOpenTab(windowId, url);
    });

    newTabAct->setDisabled(!TagHelper::workspaceServIns()->tabAddable(windowId));

    menu->addSeparator();

    // tag action
    menu->addAction(QObject::tr("Rename"), [url, windowId]() {
        TagHelper::sideBarServIns()->triggerItemEdit(windowId, url);
    });

    menu->addAction(QObject::tr("Remove"), [url]() {
        TagManager::instance()->deleteFiles({ url });
    });

    menu->addSeparator();

    TagColorListWidget *tagWidget = new TagColorListWidget;
    QWidgetAction *tagAction = new QWidgetAction(menu);
    tagAction->setDefaultWidget(tagWidget);
    tagAction->setText("Change color of present tag");
    tagWidget->setExclusive(true);
    tagWidget->setToolTipVisible(false);

    menu->addAction(tagAction);
    connect(tagAction, &QWidgetAction::triggered, TagManager::instance(), [url, tagWidget]() {
        if (tagWidget->checkedColorList().size() > 0) {
            QString tagName = TagHelper::instance()->getTagNameFromUrl(url);
            QString colorName = TagHelper::instance()->qureyColorNameByColor(tagWidget->checkedColorList().first());

            TagManager::instance()->changeTagColor(tagName, colorName);
        }
    });

    menu->exec(globalPos);
    delete menu;
}

void TagManager::renameHandle(quint64 windowId, const QUrl &url, const QString &name)
{
    Q_UNUSED(windowId);
    TagManager::instance()->changeTagName(TagHelper::instance()->getTagNameFromUrl(url), name);
}

void TagManager::onTagAdded(const QStringList &tags)
{
    for (const QString &tag : tags) {
        TagHelper::sideBarServIns()->addItem(TagHelper::instance()->createSidebarItemInfo(tag));
    }
}

void TagManager::onTagDeleted(const QStringList &tags)
{
    for (const QString &tag : tags) {
        QUrl url = TagHelper::instance()->makeTagUrlByTagName(tag);
        TagHelper::sideBarServIns()->removeItem(url);
    }
}

void TagManager::onTagColorChanged(const QMap<QString, QString> &tagAndColorName)
{
    QMap<QString, QString>::const_iterator it = tagAndColorName.begin();

    while (it != tagAndColorName.end()) {
        QUrl url = TagHelper::instance()->makeTagUrlByTagName(it.key());
        QString iconName = TagHelper::instance()->qureyIconNameByColorName(it.value());
        QIcon icon = QIcon::fromTheme(iconName);

        TagHelper::sideBarServIns()->updateItemIcon(url, icon);
        ++it;
    }
}

void TagManager::onTagNameChanged(const QMap<QString, QString> &oldAndNew)
{
    QMap<QString, QString>::const_iterator it = oldAndNew.begin();

    while (it != oldAndNew.end()) {
        QUrl url = TagHelper::instance()->makeTagUrlByTagName(it.key());
        SideBar::ItemInfo info = TagHelper::instance()->createSidebarItemInfo(it.value());

        TagHelper::sideBarServIns()->updateItem(url, info);
        ++it;
    }
}

void TagManager::onFilesTagged(const QMap<QString, QList<QString>> &fileAndTags)
{
    Q_UNUSED(fileAndTags)
}

void TagManager::onFilesUntagged(const QMap<QString, QList<QString>> &fileAndTags)
{
    Q_UNUSED(fileAndTags)
}