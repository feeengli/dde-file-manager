/*
 * Copyright (C) 2022 ~ 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     wangchunlin<wangchunlin@uniontech.com>
 *
 * Maintainer: wangchunlin<wangchunlin@uniontech.com>
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
#include "fileoperaterproxy.h"
#include "fileoperaterproxy_p.h"
#include "canvasmanager.h"
#include "view/canvasview.h"
#include "model/canvasmodel.h"
#include "grid/canvasgrid.h"
#include "model/canvasselectionmodel.h"

#include <dfm-base/dfm_event_defines.h>
#include <dfm-base/utils/clipboard.h>
#include <dfm-framework/framework.h>

#include <services/common/propertydialog/property_defines.h>
#include <services/common/bluetooth/bluetooth_defines.h>

DFMGLOBAL_USE_NAMESPACE
DFMBASE_USE_NAMESPACE
DDP_CANVAS_USE_NAMESPACE

#define KEY_SCREENNUMBER "screenNumber"
#define KEY_POINT "point"

class FileBusinessGlobal : public FileOperaterProxy
{
};
Q_GLOBAL_STATIC(FileBusinessGlobal, fileBusinessGlobal)

FileOperaterProxyPrivate::FileOperaterProxyPrivate(FileOperaterProxy *q_ptr)
    : QObject(q_ptr), q(q_ptr)
{
}

void FileOperaterProxyPrivate::callBackTouchFile(const QUrl &target, const QVariantMap &customData)
{
    QString path = target.toString();
    int screenNum = customData.value(KEY_SCREENNUMBER).toInt();
    QPoint pos = customData.value(KEY_POINT).value<QPoint>();

    // befor call back,recive file created signal
    QPair<int, QPoint> oriPoint;
    if (Q_UNLIKELY(GridIns->point(path, oriPoint))) {

        if (CanvasGrid::Mode::Align == GridIns->mode())
            return;

        if (oriPoint.first == screenNum && oriPoint.second == pos)
            return;

        // move it
        bool moved = GridIns->move(screenNum, pos, path, { path });
        qDebug() << "item:" << path << " move:" << moved << " ori:" << oriPoint.first << oriPoint.second << "   target:" << screenNum << pos;
    } else {
        if (CanvasGrid::Mode::Align == GridIns->mode())
            GridIns->append(path);
        else
            GridIns->tryAppendAfter({ path }, screenNum, pos);
    }
    CanvasIns->update();
    // need open editor,only by call back(by menu create file)
    CanvasIns->openEditor(target);
}

FileOperaterProxy::FileOperaterProxy(QObject *parent)
    : QObject(parent), d(new FileOperaterProxyPrivate(this))
{
}

FileOperaterProxy *FileOperaterProxy::instance()
{
    return fileBusinessGlobal;
}

void FileOperaterProxy::touchFile(const CanvasView *view, const QPoint pos, const dfmbase::Global::CreateFileType type, QString suffix)
{
    QVariantMap data;
    data.insert(KEY_SCREENNUMBER, view->screenNum());
    data.insert(KEY_POINT, pos);
    QPair<FileOperaterProxyPrivate::CallBackFunc, QVariant> funcData(FileOperaterProxyPrivate::kCallBackTouchFile, data);
    QVariant custom = QVariant::fromValue(funcData);

    dpfInstance.eventDispatcher().publish(GlobalEventType::kTouchFileCallBack, view->winId(), view->model()->rootUrl(), type, suffix, custom, &FileOperaterProxy::callBackFunction);
}

void FileOperaterProxy::touchFolder(const CanvasView *view, const QPoint pos)
{
    QVariantMap data;
    data.insert(KEY_SCREENNUMBER, view->screenNum());
    data.insert(KEY_POINT, pos);
    QPair<FileOperaterProxyPrivate::CallBackFunc, QVariant> funcData(FileOperaterProxyPrivate::kCallBackTouchFolder, data);
    QVariant custom = QVariant::fromValue(funcData);

    dpfInstance.eventDispatcher().publish(GlobalEventType::kMkdirCallBack, view->winId(), view->model()->rootUrl(), kCreateFileTypeFolder, custom, &FileOperaterProxy::callBackFunction);
}

void FileOperaterProxy::copyFiles(const CanvasView *view)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kWriteUrlsToClipboard, view->winId(), ClipBoard::ClipboardAction::kCopyAction, view->selectionModel()->selectedUrls());
}

void FileOperaterProxy::cutFiles(const CanvasView *view)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kWriteUrlsToClipboard, view->winId(), ClipBoard::ClipboardAction::kCutAction, view->selectionModel()->selectedUrls());
}

void FileOperaterProxy::pasteFiles(const CanvasView *view, const QPoint pos)
{
    // feature:paste at pos
    Q_UNUSED(pos)

    auto urls = ClipBoard::instance()->clipboardFileUrlList();
    ClipBoard::ClipboardAction action = ClipBoard::instance()->clipboardAction();
    if (ClipBoard::kCopyAction == action) {
        dpfInstance.eventDispatcher().publish(GlobalEventType::kCopy, view->winId(), urls, view->model()->rootUrl(), AbstractJobHandler::JobFlag::kNoHint);
    } else if (ClipBoard::kCutAction == action) {
        dpfInstance.eventDispatcher().publish(GlobalEventType::kCutFile, view->winId(), urls, view->model()->rootUrl(), AbstractJobHandler::JobFlag::kNoHint);
        //! todo bug#63441 如果是剪切操作，则禁止跨用户的粘贴操作, 讨论是否应该由下层统一处理?

        // clear clipboard after cutting files from clipboard
        ClipBoard::instance()->clearClipboard();
    } else {
        qWarning() << "clipboard action:" << action << "    urls:" << urls;
    }
}

void FileOperaterProxy::openFiles(const CanvasView *view)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kOpenFiles, view->winId(), view->selectionModel()->selectedUrls());
}

void FileOperaterProxy::openFiles(const CanvasView *view, const QList<QUrl> &urls)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kOpenFiles, view->winId(), urls);
}

void FileOperaterProxy::renameFile(const CanvasView *view, const QUrl &oldUrl, const QUrl &newUrl)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kRenameFile, view->winId(), oldUrl, newUrl);
}

void FileOperaterProxy::renameFiles(const CanvasView *view, const QList<QUrl> &urls, const QPair<QString, QString> &pair, const bool replace)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kRenameFiles, view->winId(), urls, pair, replace);
}

void FileOperaterProxy::renameFiles(const CanvasView *view, const QList<QUrl> &urls, const QPair<QString, AbstractJobHandler::FileNameAddFlag> pair)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kRenameFiles, view->winId(), urls, pair);
}

void FileOperaterProxy::openFilesByApp(const CanvasView *view)
{
    Q_UNUSED(view)
    // todo(wangcl):dependent right-click menu
}

void FileOperaterProxy::moveToTrash(const CanvasView *view)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kMoveToTrash, view->winId(), view->selectionModel()->selectedUrls(), AbstractJobHandler::JobFlag::kNoHint);
}

void FileOperaterProxy::deleteFiles(const CanvasView *view)
{
    dpfInstance.eventDispatcher().publish(GlobalEventType::kDeleteFiles, view->winId(), view->selectionModel()->selectedUrls(), AbstractJobHandler::JobFlag::kNoHint);
}

void FileOperaterProxy::showFilesProperty(const CanvasView *view)
{
    dpfInstance.eventDispatcher().publish(DSC_NAMESPACE::PropertyEventType::kEvokeDefaultFileProperty,
                                          view->selectionModel()->selectedUrls());
}

void FileOperaterProxy::sendFilesToBluetooth(const CanvasView *view)
{
    QList<QUrl> urls = view->selectionModel()->selectedUrls();
    if (!urls.isEmpty())
        dpfInstance.eventDispatcher().publish(DSC_NAMESPACE::EventType::kSendFiles, view->selectionModel()->selectedUrls());
}

void FileOperaterProxy::dropFiles(const Qt::DropAction &action, const QUrl &targetUrl, const QList<QUrl> &urls)
{
    // drop files from other app will auto append,independent of the view
    auto view = CanvasIns->views().first();
    if (action == Qt::MoveAction) {
        dpfInstance.eventDispatcher().publish(GlobalEventType::kCutFile, view->winId(), urls, targetUrl, AbstractJobHandler::JobFlag::kNoHint);
    } else {
        // default is copy file
        dpfInstance.eventDispatcher().publish(GlobalEventType::kCopy, view->winId(), urls, targetUrl, AbstractJobHandler::JobFlag::kNoHint);
    }
}

void FileOperaterProxy::dropToTrash(const QList<QUrl> &urls)
{
    auto view = CanvasIns->views().first();
    dpfInstance.eventDispatcher().publish(GlobalEventType::kMoveToTrash, view->winId(), urls, AbstractJobHandler::JobFlag::kNoHint);
}

void FileOperaterProxy::dropToApp(const QList<QUrl> &urls, const QString &app)
{
    auto view = CanvasIns->views().first();
    QList<QString> apps { app };
    dpfInstance.eventDispatcher().publish(GlobalEventType::kOpenFilesByApp, view->winId(), urls, apps);
}

void FileOperaterProxy::callBackFunction(const CallbackArgus args)
{
    const QVariant &customValue = args->value(CallbackKey::kCustom);
    const QPair<FileOperaterProxyPrivate::CallBackFunc, QVariant> &custom = customValue.value<QPair<FileOperaterProxyPrivate::CallBackFunc, QVariant>>();
    const FileOperaterProxyPrivate::CallBackFunc funcKey = custom.first;

    if (!args->value(CallbackKey::kSuccessed, false).toBool()) {
        qWarning() << "call back function by:" << funcKey << ".And it is failed.";
        return;
    }

    switch (funcKey) {
    case FileOperaterProxyPrivate::CallBackFunc::kCallBackTouchFile:
    case FileOperaterProxyPrivate::CallBackFunc::kCallBackTouchFolder: {
        // Folder also belong to files

        auto targets = args->value(CallbackKey::kTargets).value<QList<QUrl>>();
        if (Q_UNLIKELY(targets.count() != 1)) {
            qWarning() << "unknow error.touch folder successed,target url is:" << targets;
            return;
        }

        FileOperaterProxyPrivate::callBackTouchFile(targets.first(), custom.second.toMap());
    } break;
    default:
        break;
    }
}