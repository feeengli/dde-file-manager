/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yanghao<yanghao@uniontech.com>
 *
 * Maintainer: huangyu<huangyub@uniontech.com>
 *             liuyangming<liuyangming@uniontech.com>
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
#ifndef TRASHHELPER_H
#define TRASHHELPER_H

#include "dfmplugin_trash_global.h"

#include "services/filemanager/windows/windowsservice.h"
#include "services/filemanager/titlebar/titlebarservice.h"
#include "services/filemanager/sidebar/sidebarservice.h"
#include "services/filemanager/workspace/workspaceservice.h"
#include "services/common/fileoperations/fileoperationsservice.h"

#include "dfm-base/utils/clipboard.h"
#include "dfm-base/dfm_global_defines.h"

#include <QUrl>
#include <QIcon>

QT_BEGIN_HEADER
class QFrame;
QT_END_NAMESPACE

DFMBASE_BEGIN_NAMESPACE
class LocalFileWatcher;
DFMBASE_END_NAMESPACE

DPTRASH_BEGIN_NAMESPACE

class EmptyTrashWidget;
class TrashHelper final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TrashHelper)

public:
    static TrashHelper *instance();

    inline static QString scheme()
    {
        return DFMBASE_NAMESPACE::Global::kTrash;
    }

    inline static QIcon icon()
    {
        return QIcon::fromTheme("user-trash-symbolic");
    }

    static QUrl rootUrl();
    static quint64 windowId(QWidget *sender);
    static void contenxtMenuHandle(const quint64 windowId, const QUrl &url, const QPoint &globalPos);
    static QFrame *createEmptyTrashTopWidget();
    static bool showTopWidget(QWidget *w, const QUrl &url);
    static QUrl fromTrashFile(const QString &filePath);
    static QUrl fromLocalFile(const QString &filePath);
    static QUrl toLocalFile(const QUrl &url);
    static bool isEmpty();
    static void emptyTrash(const quint64 windowId = 0);

    // services instance
    static DSB_FM_NAMESPACE::WindowsService *winServIns();
    static DSB_FM_NAMESPACE::TitleBarService *titleServIns();
    static DSB_FM_NAMESPACE::SideBarService *sideBarServIns();
    static DSB_FM_NAMESPACE::WorkspaceService *workspaceServIns();
    static DSC_NAMESPACE::FileOperationsService *fileOperationsServIns();

signals:
    void trashStateChanged(bool isEmpty);

private:
    void trashFilesChanged(const QUrl &url);

private:
    explicit TrashHelper(QObject *parent = nullptr);
    void initTrashWatcher();

private:
    DFMBASE_NAMESPACE::LocalFileWatcher *trashFileWatcher { nullptr };
    bool isTrashEmpty;
};

DPTRASH_END_NAMESPACE
#endif   // TRASHHELPER_H