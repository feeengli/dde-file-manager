/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -i ../../plugins/desktop/ddplugin-dbusregister/vaultmanagerdbus.h -c VaultManagerAdaptor -l VaultManagerDBus -a ../../plugins/desktop/ddplugin-dbusregister/dbus_adaptor/vaultmanagerdbus_adaptor vaultmanagerdbus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "../../plugins/desktop/ddplugin-dbusregister/dbus_adaptor/vaultmanagerdbus_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class VaultManagerAdaptor
 */

VaultManagerAdaptor::VaultManagerAdaptor(VaultManagerDBus *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

VaultManagerAdaptor::~VaultManagerAdaptor()
{
    // destructor
}

void VaultManagerAdaptor::ClearLockEvent()
{
    // handle method call com.deepin.filemanager.service.VaultManager.ClearLockEvent
    parent()->ClearLockEvent();
}

void VaultManagerAdaptor::ComputerSleep(bool bSleep)
{
    // handle method call com.deepin.filemanager.service.VaultManager.ComputerSleep
    parent()->ComputerSleep(bSleep);
}

qulonglong VaultManagerAdaptor::GetLastestTime()
{
    // handle method call com.deepin.filemanager.service.VaultManager.GetLastestTime
    return parent()->GetLastestTime();
}

int VaultManagerAdaptor::GetLeftoverErrorInputTimes(int userID)
{
    // handle method call com.deepin.filemanager.service.VaultManager.GetLeftoverErrorInputTimes
    return parent()->GetLeftoverErrorInputTimes(userID);
}

int VaultManagerAdaptor::GetNeedWaitMinutes(int userID)
{
    // handle method call com.deepin.filemanager.service.VaultManager.GetNeedWaitMinutes
    return parent()->GetNeedWaitMinutes(userID);
}

qulonglong VaultManagerAdaptor::GetSelfTime()
{
    // handle method call com.deepin.filemanager.service.VaultManager.GetSelfTime
    return parent()->GetSelfTime();
}

bool VaultManagerAdaptor::IsLockEventTriggered()
{
    // handle method call com.deepin.filemanager.service.VaultManager.IsLockEventTriggered
    return parent()->IsLockEventTriggered();
}

void VaultManagerAdaptor::LeftoverErrorInputTimesMinusOne(int userID)
{
    // handle method call com.deepin.filemanager.service.VaultManager.LeftoverErrorInputTimesMinusOne
    parent()->LeftoverErrorInputTimesMinusOne(userID);
}

void VaultManagerAdaptor::RestoreLeftoverErrorInputTimes(int userID)
{
    // handle method call com.deepin.filemanager.service.VaultManager.RestoreLeftoverErrorInputTimes
    parent()->RestoreLeftoverErrorInputTimes(userID);
}

void VaultManagerAdaptor::RestoreNeedWaitMinutes(int userID)
{
    // handle method call com.deepin.filemanager.service.VaultManager.RestoreNeedWaitMinutes
    parent()->RestoreNeedWaitMinutes(userID);
}

void VaultManagerAdaptor::SetRefreshTime(qulonglong time)
{
    // handle method call com.deepin.filemanager.service.VaultManager.SetRefreshTime
    parent()->SetRefreshTime(time);
}

void VaultManagerAdaptor::StartTimerOfRestorePasswordInput(int userID)
{
    // handle method call com.deepin.filemanager.service.VaultManager.StartTimerOfRestorePasswordInput
    parent()->StartTimerOfRestorePasswordInput(userID);
}

void VaultManagerAdaptor::SysUserChanged(const QString &curUser)
{
    // handle method call com.deepin.filemanager.service.VaultManager.SysUserChanged
    parent()->SysUserChanged(curUser);
}

void VaultManagerAdaptor::TriggerLockEvent()
{
    // handle method call com.deepin.filemanager.service.VaultManager.TriggerLockEvent
    parent()->TriggerLockEvent();
}
