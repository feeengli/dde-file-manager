/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -i ../../plugins/desktop/core/ddplugin-dbusregister/vaultmanagerdbus.h -c VaultManagerAdaptor -l VaultManagerDBus -a ../../plugins/desktop/core/ddplugin-dbusregister/dbus_adaptor/vaultmanagerdbus_adaptor vaultmanagerdbus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef VAULTMANAGERDBUS_ADAPTOR_H
#define VAULTMANAGERDBUS_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "../../plugins/desktop/core/ddplugin-dbusregister/vaultmanagerdbus.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T>
class QList;
template<class Key, class Value>
class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.deepin.filemanager.service.VaultManager
 */
class VaultManagerAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.filemanager.service.VaultManager")
    Q_CLASSINFO("D-Bus Introspection", ""
                                       "  <interface name=\"com.deepin.filemanager.service.VaultManager\">\n"
                                       "    <signal name=\"lockEventTriggered\">\n"
                                       "      <arg direction=\"out\" type=\"s\" name=\"user\"/>\n"
                                       "    </signal>\n"
                                       "    <method name=\"SysUserChanged\">\n"
                                       "      <arg direction=\"in\" type=\"s\" name=\"curUser\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"SetRefreshTime\">\n"
                                       "      <arg direction=\"in\" type=\"t\" name=\"time\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"GetLastestTime\">\n"
                                       "      <arg direction=\"out\" type=\"t\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"GetSelfTime\">\n"
                                       "      <arg direction=\"out\" type=\"t\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"IsLockEventTriggered\">\n"
                                       "      <arg direction=\"out\" type=\"b\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"TriggerLockEvent\"/>\n"
                                       "    <method name=\"ClearLockEvent\"/>\n"
                                       "    <method name=\"ComputerSleep\">\n"
                                       "      <arg direction=\"in\" type=\"b\" name=\"bSleep\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"GetLeftoverErrorInputTimes\">\n"
                                       "      <arg direction=\"out\" type=\"i\"/>\n"
                                       "      <arg direction=\"in\" type=\"i\" name=\"userID\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"LeftoverErrorInputTimesMinusOne\">\n"
                                       "      <arg direction=\"in\" type=\"i\" name=\"userID\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"RestoreLeftoverErrorInputTimes\">\n"
                                       "      <arg direction=\"in\" type=\"i\" name=\"userID\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"StartTimerOfRestorePasswordInput\">\n"
                                       "      <arg direction=\"in\" type=\"i\" name=\"userID\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"GetNeedWaitMinutes\">\n"
                                       "      <arg direction=\"out\" type=\"i\"/>\n"
                                       "      <arg direction=\"in\" type=\"i\" name=\"userID\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"RestoreNeedWaitMinutes\">\n"
                                       "      <arg direction=\"in\" type=\"i\" name=\"userID\"/>\n"
                                       "    </method>\n"
                                       "  </interface>\n"
                                       "")
public:
    VaultManagerAdaptor(VaultManagerDBus *parent);
    virtual ~VaultManagerAdaptor();

    inline VaultManagerDBus *parent() const
    {
        return static_cast<VaultManagerDBus *>(QObject::parent());
    }

public:   // PROPERTIES
public Q_SLOTS:   // METHODS
    void ClearLockEvent();
    void ComputerSleep(bool bSleep);
    qulonglong GetLastestTime();
    int GetLeftoverErrorInputTimes(int userID);
    int GetNeedWaitMinutes(int userID);
    qulonglong GetSelfTime();
    bool IsLockEventTriggered();
    void LeftoverErrorInputTimesMinusOne(int userID);
    void RestoreLeftoverErrorInputTimes(int userID);
    void RestoreNeedWaitMinutes(int userID);
    void SetRefreshTime(qulonglong time);
    void StartTimerOfRestorePasswordInput(int userID);
    void SysUserChanged(const QString &curUser);
    void TriggerLockEvent();
Q_SIGNALS:   // SIGNALS
    void lockEventTriggered(const QString &user);
};

#endif