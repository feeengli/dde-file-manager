/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     xushitong<xushitong@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             zhangsheng<zhangsheng@uniontech.com>
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
#include "smbbrowsermenuscene.h"
#include "private/smbbrowsermenuscene_p.h"

#include "services/common/menu/menu_defines.h"
#include "dfm-base/base/device/devicecontroller.h"
#include "dfm-base/utils/dialogmanager.h"
#include "dfm-base/dfm_event_defines.h"

#include <dfm-framework/framework.h>

#include <QMenu>

DPSMBBROWSER_USE_NAMESPACE
DFMBASE_USE_NAMESPACE

AbstractMenuScene *SmbBrowserMenuCreator::create()
{
    return new SmbBrowserMenuScene();
}

SmbBrowserMenuScene::SmbBrowserMenuScene(QObject *parent)
    : AbstractMenuScene(parent), d(new SmbBrowserMenuScenePrivate(this))
{
}

SmbBrowserMenuScene::~SmbBrowserMenuScene()
{
}

QString SmbBrowserMenuScene::name() const
{
    return SmbBrowserMenuCreator::name();
}

bool SmbBrowserMenuScene::initialize(const QVariantHash &params)
{
    DSC_USE_NAMESPACE
    d->selectFiles = params.value(MenuParamKey::kSelectFiles).value<QList<QUrl>>();
    d->isEmptyArea = params.value(MenuParamKey::kIsEmptyArea).toBool();
    d->windowId = params.value(MenuParamKey::kWindowId).toULongLong();
    if (d->selectFiles.count() != 1 || d->isEmptyArea)
        return false;
    return true;
}

bool SmbBrowserMenuScene::create(QMenu *parent)
{
    DSC_USE_NAMESPACE
    auto act = parent->addAction(d->predicateName[SmbBrowserActionId::kOpenSmb]);
    act->setProperty(ActionPropertyKey::kActionID, SmbBrowserActionId::kOpenSmb);
    d->predicateAction[SmbBrowserActionId::kOpenSmb] = act;
    parent->addSeparator();

    act = parent->addAction(d->predicateName[SmbBrowserActionId::kOpenSmbInNewWin]);
    act->setProperty(ActionPropertyKey::kActionID, SmbBrowserActionId::kOpenSmbInNewWin);
    d->predicateAction[SmbBrowserActionId::kOpenSmbInNewWin] = act;

    return true;
}

void SmbBrowserMenuScene::updateState(QMenu *parent)
{
}

bool SmbBrowserMenuScene::triggered(QAction *action)
{
    DSC_USE_NAMESPACE
    const QString &actId = action->property(ActionPropertyKey::kActionID).toString();
    if (!d->predicateAction.contains(actId))
        return false;

    if (d->selectFiles.count() != 1)
        return false;

    quint64 winId = d->windowId;
    const QString &smbUrl = d->selectFiles.first().toString();
    DeviceController::instance()->mountNetworkDevice(smbUrl, [actId, winId](bool ok, dfmmount::DeviceError err, const QString &mntPath) {
        if (!ok && err != DFMMOUNT::DeviceError::GIOErrorAlreadyMounted) {
            DialogManagerInstance->showErrorDialogWhenMountDeviceFailed(err);
        } else {
            QUrl u = QUrl::fromLocalFile(mntPath);
            if (actId == SmbBrowserActionId::kOpenSmb)
                dpfInstance.eventDispatcher().publish(GlobalEventType::kChangeCurrentUrl, winId, u);
            else
                dpfInstance.eventDispatcher().publish(GlobalEventType::kOpenNewWindow, u);
        }
    });
    return true;
}

AbstractMenuScene *SmbBrowserMenuScene::scene(QAction *action) const
{
    if (action == nullptr)
        return nullptr;

    if (!d->predicateAction.key(action).isEmpty())
        return const_cast<SmbBrowserMenuScene *>(this);

    return AbstractMenuScene::scene(action);
}

SmbBrowserMenuScenePrivate::SmbBrowserMenuScenePrivate(AbstractMenuScene *qq)
    : AbstractMenuScenePrivate(qq)
{
    predicateName[SmbBrowserActionId::kOpenSmb] = tr("Open");
    predicateName[SmbBrowserActionId::kOpenSmbInNewWin] = tr("Open in new window");
}