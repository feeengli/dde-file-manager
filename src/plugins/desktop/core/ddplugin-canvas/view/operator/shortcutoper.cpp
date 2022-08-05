/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangyu<zhangyub@uniontech.com>
 *
 * Maintainer: zhangyu<zhangyub@uniontech.com>
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
#include "shortcutoper.h"
#include "view/canvasview.h"
#include "model/canvasselectionmodel.h"
#include "delegate/canvasitemdelegate.h"
#include "view/canvasview_p.h"
#include "fileoperatorproxy.h"
#include "canvasmanager.h"
#include "utils/fileutil.h"

#include <dfm-base/base/application/application.h>
#include <dfm-base/base/application/settings.h>
#include <dfm-base/utils/clipboard.h>
#include <dfm-base/base/schemefactory.h>

#include <dfm-framework/event/event.h>

#include <DApplication>
#include <QAction>

DFMBASE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace ddplugin_canvas;

#define regAction(shortcut, autoRepeat)                                             \
    {                                                                               \
        QAction *action = new QAction(view);                                        \
        action->setAutoRepeat(autoRepeat);                                          \
        action->setShortcut(shortcut);                                              \
        action->setProperty("_view_shortcut_key", shortcut);                        \
        view->addAction(action);                                                    \
        connect(action, &QAction::triggered, this, &ShortcutOper::acitonTriggered); \
    }

#define actionShortcutKey(action) action->property("_view_shortcut_key").value<QKeySequence::StandardKey>()

ShortcutOper::ShortcutOper(CanvasView *parent)
    : QObject(parent), view(parent)
{
}

void ShortcutOper::regShortcut()
{
    regAction(QKeySequence::HelpContents, true);   // F1
    regAction(QKeySequence::Refresh, true);   // F5
    regAction(QKeySequence::Delete, true);   // Del
    regAction(QKeySequence::SelectAll, true);   // ctrl+a
    regAction(QKeySequence::ZoomIn, true);   // ctrl+-
    regAction(QKeySequence::ZoomOut, true);   // ctrl++(c_s_=)
    regAction(QKeySequence::Copy, false);   // ctrl+c
    regAction(QKeySequence::Cut, false);   // ctrl+x
    regAction(QKeySequence::Paste, false);   // ctrl+v
    regAction(QKeySequence::Undo, true);   // ctrl+z
    //regAction(QKeySequence::New, [this](){qDebug() << "New";});// ctrl+n
    //regAction(QKeySequence::Open, &ShortcutOper::openAction); // ctrl+o
}

bool ShortcutOper::keyPressed(QKeyEvent *event)
{
    if (!event || !view)
        return false;

    {
        QVariantHash extData;
        extData.insert("DisableShortcut", disableShortcut());
        extData.insert("QKeyEvent", (qlonglong)event);
        if (view->d->hookIfs && view->d->hookIfs->shortcutkeyPress(view->screenNum(), event->key(), event->modifiers(), &extData))
            return true;
    }

    if (disableShortcut()) {
        bool specialShortcut = false;
        if (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier) {
            switch (event->key()) {
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                specialShortcut = true;
                break;
            default:
                specialShortcut = false;   // other keys is disable
            }
        }
        if (!specialShortcut)
            return true;   // return true to ingore the event.
    }

    Qt::KeyboardModifiers modifiers = event->modifiers();
    auto key = event->key();
    if (modifiers == Qt::NoModifier) {
        switch (key) {
        case Qt::Key_Tab:
            tabToFirst();
            return true;
        case Qt::Key_Escape:
            clearClipBoard();
        default:
            break;
        }
    }

    if (modifiers == Qt::KeypadModifier || modifiers == Qt::NoModifier) {
        switch (key) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            FileOperatorProxyIns->openFiles(view);
            return true;
        case Qt::Key_Space:
            if (!event->isAutoRepeat())
                previewFiles();
            break;
        default:
            break;
        }
    } else if (modifiers == Qt::ShiftModifier) {
        switch (key) {
        case Qt::Key_Delete:
            FileOperatorProxyIns->deleteFiles(view);
            break;
        case Qt::Key_T:
            // open terminal. but no need to do it.
            return true;
        default:
            break;
        }
    } else if (modifiers == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_Equal:
            CanvasIns->onChangeIconLevel(true);
            return true;
        case Qt::Key_H:
            swichHidden();
            return true;
        case Qt::Key_I:
            FileOperatorProxyIns->showFilesProperty(view);
            return true;
        default:
            break;
        }
    } else if (modifiers == Qt::AltModifier) {
        if (key == Qt::Key_M) {
            showMenu();
            return true;
        }
    }

    return false;
}

bool ShortcutOper::disableShortcut() const
{
    return DFMBASE_NAMESPACE::Application::appObtuselySetting()->value(
                                                                       "ApplicationAttribute", "DisableDesktopShortcuts", false)
            .toBool();
}

void ShortcutOper::acitonTriggered()
{
    QAction *ac = qobject_cast<QAction *>(sender());
    if (!ac)
        return;

    auto key = actionShortcutKey(ac);
    {
        QVariantHash extData;
        extData.insert("DisableShortcut", disableShortcut());
        if (view->d->hookIfs && view->d->hookIfs->shortcutAction(view->screenNum(), key, &extData))
            return;
    }

    switch (key) {
    case QKeySequence::Copy:
        FileOperatorProxyIns->copyFiles(view);
        break;
    case QKeySequence::Cut:
        FileOperatorProxyIns->cutFiles(view);
        break;
    case QKeySequence::Paste:
        FileOperatorProxyIns->pasteFiles(view);
        break;
    case QKeySequence::Undo:
        qDebug() << "Undo";
        FileOperatorProxyIns->undoFiles(view);
        break;
    default:
        break;
    }

    if (!disableShortcut()) {
        switch (key) {
        case QKeySequence::HelpContents:
            helpAction();
            break;
        case QKeySequence::Refresh:
            view->refresh();
            break;
        case QKeySequence::Delete:
            FileOperatorProxyIns->moveToTrash(view);
            break;
        case QKeySequence::SelectAll:
            view->selectAll();
            break;
        case QKeySequence::ZoomIn:
            CanvasIns->onChangeIconLevel(true);
            break;
        case QKeySequence::ZoomOut:
            CanvasIns->onChangeIconLevel(false);
            break;
        default:
            break;
        }
    }
}

void ShortcutOper::helpAction()
{
    class PublicApplication : public DApplication
    {
    public:
        using DApplication::handleHelpAction;
    };

    QString appName = qApp->applicationName();
    qApp->setApplicationName("dde");
    reinterpret_cast<PublicApplication *>(DApplication::instance())->handleHelpAction();
    qApp->setApplicationName(appName);
}

void ShortcutOper::tabToFirst()
{
    view->selectionModel()->clear();
    QKeyEvent downKey(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QCoreApplication::sendEvent(view, &downKey);
}

void ShortcutOper::showMenu()
{
    if (CanvasViewMenuProxy::disableMenu())
        return;

    QModelIndexList indexList = view->selectionModel()->selectedIndexes();
    bool isEmptyArea = indexList.isEmpty();
    Qt::ItemFlags flags;
    QModelIndex index;
    if (isEmptyArea) {
        index = view->rootIndex();
        flags = view->model()->flags(index);
        if (!flags.testFlag(Qt::ItemIsEnabled))
            return;
    } else {
        index = view->currentIndex();

        // the current index may be not in selected indexs."
        if (!indexList.contains(index)) {
            qDebug() << "current index is not selected.";
            index = indexList.last();
        }

        flags = view->model()->flags(index);
        if (!flags.testFlag(Qt::ItemIsEnabled)) {
            qInfo() << "file is disbale, switch to empty area" << view->model()->fileUrl(index);
            isEmptyArea = true;
            flags = view->rootIndex().flags();
        }
    }

    view->itemDelegate()->revertAndcloseEditor();
    if (isEmptyArea) {
        view->selectionModel()->clearSelection();
        view->d->menuProxy->showEmptyAreaMenu(flags, QPoint(0, 0));
    } else {
        auto gridPos = view->d->gridAt(view->visualRect(index).center());
        view->d->menuProxy->showNormalMenu(index, flags, gridPos);
    }
}

void ShortcutOper::clearClipBoard()
{
    auto urls = ClipBoard::instance()->clipboardFileUrlList();
    auto homePath = view->model()->rootUrl();
    if (!urls.isEmpty()) {
        auto itemInfo = FileCreator->createFileInfo(urls.first(), false);
        if (itemInfo && (itemInfo->absolutePath() == homePath.toLocalFile()))
            ClipBoard::instance()->clearClipboard();
    }
}

void ShortcutOper::swichHidden()
{
    auto model = view->model();
    Q_ASSERT(model);

    bool show = model->showHiddenFiles();
    model->setShowHiddenFiles(!show);
    model->refresh(model->rootIndex());
}

void ShortcutOper::previewFiles()
{
    QList<QUrl> urls = view->selectionModel()->selectedUrls();
    if (urls.isEmpty())
        return;

    QList<QUrl> selectUrls;
    for (QUrl &url : urls) {
        selectUrls.append(UrlRoute::fromLocalFile(url.path()));
    }
    urls = view->selectionModel()->selectedUrls();
    QList<QUrl> currentDirUrls;
    for (QUrl &url : urls) {
        currentDirUrls.append(UrlRoute::fromLocalFile(url.path()));
    }
    dpfSlotChannel->push("dfmplugin_filepreview", "slot_PreviewDialog_Show", view->topLevelWidget()->winId(), selectUrls, currentDirUrls);
}