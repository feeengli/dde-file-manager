/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lixiang<lixianga@uniontech.com>
 *
 * Maintainer: lixiang<lixianga@uniontech.com>
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
#include "vaultvisiblemanager.h"
#include "utils/policy/policymanager.h"
#include "fileutils/vaultfileinfo.h"
#include "utils/vaulthelper.h"
#include "fileutils/vaultfileiterator.h"
#include "fileutils/vaultfilewatcher.h"
#include "utils/vaultdefine.h"
#include "utils/vaultentryfileentity.h"
#include "events/vaulteventreceiver.h"
#include "events/vaulteventcaller.h"
#include "utils/policy/policymanager.h"
#include "utils/servicemanager.h"
#include "menus/vaultmenuscene.h"
#include "menus/vaultcomputermenuscene.h"

#include "plugins/common/dfmplugin-menu/menu_eventinterface_helper.h"

#include "dfm-base/base/schemefactory.h"
#include "dfm-base/base/application/application.h"
#include "dfm-base/file/entry/entities/abstractentryfileentity.h"
#include "dfm-base/widgets/dfmwindow/filemanagerwindow.h"

#include <dfm-framework/event/event.h>

#include <DSysInfo>

using BasicViewFieldFunc = std::function<QMap<QString, QMultiMap<QString, QPair<QString, QString>>>(const QUrl &url)>;
using CustomViewExtensionView = std::function<QWidget *(const QUrl &url)>;
using ItemClickedActionCallback = std::function<void(quint64 windowId, const QUrl &url)>;
using ContextMenuCallback = std::function<void(quint64 windowId, const QUrl &url, const QPoint &globalPos)>;
Q_DECLARE_METATYPE(ItemClickedActionCallback);
Q_DECLARE_METATYPE(ContextMenuCallback);
Q_DECLARE_METATYPE(CustomViewExtensionView)
Q_DECLARE_METATYPE(BasicViewFieldFunc)

DFMBASE_USE_NAMESPACE
DPF_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace dfmplugin_vault;
VaultVisibleManager::VaultVisibleManager(QObject *parent)
    : QObject(parent)
{
}

bool VaultVisibleManager::isVaultEnabled()
{
    if (!DSysInfo::isCommunityEdition()) {   // 如果不是社区版
        DSysInfo::DeepinType deepinType = DSysInfo::deepinType();
        // 如果是专业版
        if (DSysInfo::DeepinType::DeepinProfessional == deepinType && PolicyManager::isVaultVisiable()) {
            return true;
        }
    }
    return false;
}

void VaultVisibleManager::infoRegister()
{
    PolicyManager::instance()->slotVaultPolicy();
    if (isVaultEnabled() && !infoRegisterState) {
        UrlRoute::regScheme(VaultHelper::instance()->scheme(), "/", VaultHelper::instance()->icon(), true, tr("My Vault"));

        //注册Scheme为"vault"的扩展的文件信息
        InfoFactory::regClass<VaultFileInfo>(VaultHelper::instance()->scheme());
        WatcherFactory::regClass<VaultFileWatcher>(VaultHelper::instance()->scheme());
        DirIteratorFactory::regClass<VaultFileIterator>(VaultHelper::instance()->scheme());
        EntryEntityFactor::registCreator<VaultEntryFileEntity>("vault");
        infoRegisterState = true;
    }
}

void VaultVisibleManager::pluginServiceRegister()
{
    if (!serviceRegisterState) {
        connect(&FMWindowsIns, &FileManagerWindowsManager::windowOpened, this, &VaultVisibleManager::onWindowOpened, Qt::DirectConnection);
        VaultEventReceiver::instance()->connectEvent();
        serviceRegisterState = true;
    }

    if (isVaultEnabled()) {
        dpfSlotChannel->push("dfmplugin_workspace", "slot_RegisterFileView", VaultHelper::instance()->scheme());
        dpfSlotChannel->push("dfmplugin_workspace", "slot_RegisterMenuScene", VaultHelper::instance()->scheme(), VaultMenuSceneCreator::name());

        CustomViewExtensionView customView { VaultHelper::createVaultPropertyDialog };
        dpfSlotChannel->push("dfmplugin_propertydialog", "slot_CustomView_Register",
                             customView, VaultHelper::instance()->scheme());

        BasicViewFieldFunc func { ServiceManager::basicViewFieldFunc };
        dpfSlotChannel->push("dfmplugin_propertydialog", "slot_BasicViewExtension_Register",
                             func, VaultHelper::instance()->scheme());
        QStringList &&filters { "kPermission" };
        dpfSlotChannel->push("dfmplugin_propertydialog", "slot_BasicFiledFilter_Add",
                             VaultHelper::instance()->scheme(), filters);

        dfmplugin_menu_util::menuSceneRegisterScene(VaultComputerMenuCreator::name(), new VaultComputerMenuCreator());
        dfmplugin_menu_util::menuSceneBind(VaultComputerMenuCreator::name(), "ComputerMenu");
        dfmplugin_menu_util::menuSceneRegisterScene(VaultMenuSceneCreator::name(), new VaultMenuSceneCreator);
    }
}

void VaultVisibleManager::addSideBarVaultItem()
{
    if (isVaultEnabled()) {
        ItemClickedActionCallback cdCb { VaultHelper::siderItemClicked };
        ContextMenuCallback contextMenuCb { VaultHelper::contenxtMenuHandle };
        Qt::ItemFlags flags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };
        QVariantMap map {
            { "Property_Key_Group", "Group_Device" },
            { "Property_Key_DisplayName", tr("My Vault") },
            { "Property_Key_Icon", VaultHelper::instance()->icon() },
            { "Property_Key_QtItemFlags", QVariant::fromValue(flags) },
            { "Property_Key_CallbackItemClicked", QVariant::fromValue(cdCb) },
            { "Property_Key_CallbackContextMenu", QVariant::fromValue(contextMenuCb) }
        };

        dpfSlotChannel->push("dfmplugin_sidebar", "slot_Item_Insert", 1, VaultHelper::instance()->rootUrl(), map);
    }
}

void VaultVisibleManager::addComputer()
{
    if (isVaultEnabled()) {
        dpfSlotChannel->push("dfmplugin_computer", "slot_AddDevice", tr("Vault"), QUrl("entry:///vault.vault"));
    }
}

void VaultVisibleManager::onWindowOpened(quint64 winID)
{
    auto window = FMWindowsIns.findWindowById(winID);

    if (window->sideBar())
        addSideBarVaultItem();
    else
        connect(window, &FileManagerWindow::sideBarInstallFinished, this, &VaultVisibleManager::addSideBarVaultItem, Qt::DirectConnection);

    if (window->workSpace())
        addComputer();
    else
        connect(window, &FileManagerWindow::workspaceInstallFinished, this, &VaultVisibleManager::addComputer, Qt::DirectConnection);

    VaultEventCaller::sendBookMarkDisabled(VaultHelper::instance()->scheme());
}

void VaultVisibleManager::removeSideBarVaultItem()
{
    dpfSlotChannel->push("dfmplugin_sidebar", "slot_Item_Remove", VaultHelper::instance()->rootUrl());
}

void VaultVisibleManager::removeComputerVaultItem()
{
    dpfSlotChannel->push("dfmplugin_computer", "slot_RemoveDevice", QUrl("entry:///vault.vault"));
}

VaultVisibleManager *VaultVisibleManager::instance()
{
    static VaultVisibleManager obj;
    return &obj;
}