/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c VaultInterface -p dbusinterface/vault_interface vault.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef VAULT_INTERFACE_H
#define VAULT_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.deepin.filemanager.daemon.VaultManager
 */
class VaultInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.deepin.filemanager.daemon.VaultManager"; }

public:
    VaultInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~VaultInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<qulonglong> getLastestTime()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getLastestTime"), argumentList);
    }

    inline QDBusPendingReply<qulonglong> getSelfTime()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getSelfTime"), argumentList);
    }

    inline QDBusPendingReply<> setRefreshTime(qulonglong time)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(time);
        return asyncCallWithArgumentList(QStringLiteral("setRefreshTime"), argumentList);
    }

    inline QDBusPendingReply<bool> checkAuthentication(QString type)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(type);
        return asyncCallWithArgumentList(QStringLiteral("checkAuthentication"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace com {
  namespace deepin {
    namespace filemanager {
      namespace daemon {
        typedef ::VaultInterface VaultManager;
      }
    }
  }
}
#endif
