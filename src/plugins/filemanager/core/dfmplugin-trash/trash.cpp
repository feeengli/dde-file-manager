/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yanghao<yanghao@uniontech.com>
 *
 * Maintainer: liuyangming<liuyangming@uniontech.com>
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
#include "trash.h"
#include "trashfileinfo.h"
#include "trashdiriterator.h"
#include "trashfilewatcher.h"
#include "utils/trashhelper.h"
#include "utils/trashfilehelper.h"
#include "menus/trashmenuscene.h"

#include "plugins/common/core/dfmplugin-menu/menu_eventinterface_helper.h"

#include "dfm-base/base/urlroute.h"
#include "dfm-base/base/schemefactory.h"

using BasicViewFieldFunc = std::function<QMap<QString, QMultiMap<QString, QPair<QString, QString>>>(const QUrl &url)>;
using ContextMenuCallback = std::function<void(quint64 windowId, const QUrl &url, const QPoint &globalPos)>;
using CreateTopWidgetCallback = std::function<QWidget *()>;
using ShowTopWidgetCallback = std::function<bool(QWidget *, const QUrl &)>;
Q_DECLARE_METATYPE(CreateTopWidgetCallback);
Q_DECLARE_METATYPE(ShowTopWidgetCallback);
Q_DECLARE_METATYPE(ContextMenuCallback)
Q_DECLARE_METATYPE(Qt::DropAction *)
Q_DECLARE_METATYPE(QList<QUrl> *)
Q_DECLARE_METATYPE(BasicViewFieldFunc)
Q_DECLARE_METATYPE(QString *);
Q_DECLARE_METATYPE(QVariant *)

DFMBASE_USE_NAMESPACE
using namespace dfmplugin_trash;

void Trash::initialize()
{
    UrlRoute::regScheme(TrashHelper::scheme(), "/", TrashHelper::icon(), true, tr("Trash"));
    InfoFactory::regClass<TrashFileInfo>(TrashHelper::scheme());
    WatcherFactory::regClass<TrashFileWatcher>(TrashHelper::scheme());
    DirIteratorFactory::regClass<TrashDirIterator>(TrashHelper::scheme());

    connect(&FMWindowsIns, &FileManagerWindowsManager::windowOpened, this, &Trash::onWindowOpened, Qt::DirectConnection);
}

bool Trash::start()
{
    dfmplugin_menu_util::menuSceneRegisterScene(TrashMenuCreator::name(), new TrashMenuCreator());

    dpfSlotChannel->push("dfmplugin_workspace", "slot_RegisterFileView", TrashHelper::scheme());
    dpfSlotChannel->push("dfmplugin_workspace", "slot_RegisterMenuScene", TrashHelper::scheme(), TrashMenuCreator::name());

    addCustomTopWidget();
    addFileOperations();

    dpfHookSequence->follow("dfmplugin_workspace", "hook_DragDrop_CheckDragDropAction", TrashHelper::instance(), &TrashHelper::checkDragDropAction);
    dpfHookSequence->follow("dfmplugin_detailspace", "hook_Icon_Fetch", TrashHelper::instance(), &TrashHelper::detailViewIcon);
    dpfHookSequence->follow("dfmplugin_workspace", "hook_Model_FetchCustomColumnRoles", TrashHelper::instance(), &TrashHelper::customColumnRole);
    dpfHookSequence->follow("dfmplugin_workspace", "hook_Model_FetchCustomRoleDisplayName", TrashHelper::instance(), &TrashHelper::customRoleDisplayName);
    dpfHookSequence->follow("dfmplugin_workspace", "hook_Model_FetchCustomRoleData", TrashHelper::instance(), &TrashHelper::customRoleData);
    dpfHookSequence->follow("dfmplugin_utils", "hook_UrlsTransform", TrashHelper::instance(), &TrashHelper::urlsToLocal);

    // hook events, file operation
    dpfHookSequence->follow("dfmplugin_fileoperations", "hook_Operation_CutFile", TrashFileHelper::instance(), &TrashFileHelper::cutFile);
    dpfHookSequence->follow("dfmplugin_fileoperations", "hook_Operation_CopyFile", TrashFileHelper::instance(), &TrashFileHelper::copyFile);
    dpfHookSequence->follow("dfmplugin_fileoperations", "hook_Operation_MoveToTrash", TrashFileHelper::instance(), &TrashFileHelper::moveToTrash);
    dpfHookSequence->follow("dfmplugin_fileoperations", "hook_Operation_DeleteFile", TrashFileHelper::instance(), &TrashFileHelper::deleteFile);
    dpfHookSequence->follow("dfmplugin_fileoperations", "hook_Operation_OpenFileInPlugin", TrashFileHelper::instance(), &TrashFileHelper::openFileInPlugin);
    dpfHookSequence->follow("dfmplugin_fileoperations", "hook_Operation_WriteUrlsToClipboard", TrashFileHelper::instance(), &TrashFileHelper::writeUrlsToClipboard);

    return true;
}

dpf::Plugin::ShutdownFlag Trash::stop()
{
    return kSync;
}

void Trash::onWindowOpened(quint64 windId)
{
    auto window = FMWindowsIns.findWindowById(windId);
    Q_ASSERT_X(window, "Trash", "Cannot find window by id");
    if (window->titleBar())
        regTrashCrumbToTitleBar();
    else
        connect(window, &FileManagerWindow::titleBarInstallFinished, this, &Trash::regTrashCrumbToTitleBar, Qt::DirectConnection);

    if (window->sideBar())
        installToSideBar();
    else
        connect(window, &FileManagerWindow::sideBarInstallFinished, this, [this] { installToSideBar(); }, Qt::DirectConnection);
}

void Trash::regTrashCrumbToTitleBar()
{
    static std::once_flag flag;
    std::call_once(flag, []() {
        dpfSlotChannel->push("dfmplugin_titlebar", "slot_Custom_Register", TrashHelper::scheme(), QVariantMap {});
    });
}

void Trash::installToSideBar()
{
    ContextMenuCallback contextMenuCb { TrashHelper::contenxtMenuHandle };

    Qt::ItemFlags flags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };
    QVariantMap map {
        { "Property_Key_Group", "Group_Common" },
        { "Property_Key_DisplayName", tr("Trash") },
        { "Property_Key_Icon", TrashHelper::icon() },
        { "Property_Key_QtItemFlags", QVariant::fromValue(flags) },
        { "Property_Key_CallbackContextMenu", QVariant::fromValue(contextMenuCb) },
        { "Property_Key_VisiableControl", "trash" }
    };
    dpfSlotChannel->push("dfmplugin_sidebar", "slot_Item_Add", TrashHelper::rootUrl(), map);
}

void Trash::addFileOperations()
{
    BasicViewFieldFunc func { TrashHelper::propetyExtensionFunc };
    dpfSlotChannel->push("dfmplugin_propertydialog", "slot_BasicViewExtension_Register",
                         func, TrashHelper::scheme());

    QStringList &&filtes { "kPermission" };
    dpfSlotChannel->push("dfmplugin_propertydialog", "slot_BasicFiledFilter_Add",
                         TrashHelper::scheme(), filtes);
}

void Trash::addCustomTopWidget()
{
    CreateTopWidgetCallback createCallback { TrashHelper::createEmptyTrashTopWidget };
    ShowTopWidgetCallback showCallback { TrashHelper::showTopWidget };

    QVariantMap map {
        { "Property_Key_Scheme", TrashHelper::scheme() },
        { "Property_Key_KeepShow", false },
        { "Property_Key_CreateTopWidgetCallback", QVariant::fromValue(createCallback) },
        { "Property_Key_ShowTopWidgetCallback", QVariant::fromValue(showCallback) }
    };

    dpfSlotChannel->push("dfmplugin_workspace", "slot_RegisterCustomTopWidget", map);
}