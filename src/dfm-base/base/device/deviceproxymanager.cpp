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
#include "deviceproxymanager.h"
#include "devicemanager.h"
#include "deviceutils.h"
#include "private/deviceproxymanager_p.h"

#include "dfm-base/dbusservice/dbus_interface/devicemanagerdbus_interface.h"

#include <QDBusServiceWatcher>

using namespace dfmbase;
static constexpr char kDesktopService[] { "com.deepin.filemanager.service" };
static constexpr char kDevMngPath[] { "/com/deepin/filemanager/service/DeviceManager" };
static constexpr char kDevMngIFace[] { "com.deepin.filemanager.service.DeviceManager" };

const DeviceManagerInterface *DeviceProxyManager::getDBusIFace() const
{
    return d->devMngDBus.data();
}

QStringList DeviceProxyManager::getAllBlockIds(GlobalServerDefines::DeviceQueryOptions opts)
{
    if (d->isDBusRuning()) {
        auto &&reply = d->devMngDBus->GetBlockDevicesIdList(opts);
        reply.waitForFinished();
        return reply.value();
    } else {
        return DevMngIns->getAllBlockDevID(opts);
    }
}

QStringList DeviceProxyManager::getAllBlockIdsByUUID(const QStringList &uuids, GlobalServerDefines::DeviceQueryOptions opts)
{
    const auto &&devices = getAllBlockIds(opts);
    QStringList devs;
    for (const auto &id : devices) {
        const auto &&info = queryBlockInfo(id);
        if (uuids.contains(info.value(GlobalServerDefines::DeviceProperty::kUUID).toString()))
            devs << id;
    }
    return devs;
}

QStringList DeviceProxyManager::getAllProtocolIds()
{
    if (d->isDBusRuning()) {
        auto &&reply = d->devMngDBus->GetProtocolDevicesIdList();
        reply.waitForFinished();
        return reply.value();
    } else {
        return DevMngIns->getAllProtocolDevID();
    }
}

QVariantMap DeviceProxyManager::queryBlockInfo(const QString &id, bool reload)
{
    if (d->isDBusRuning()) {
        auto &&reply = d->devMngDBus->QueryBlockDeviceInfo(id, reload);
        reply.waitForFinished();
        return reply.value();
    } else {
        return DevMngIns->getBlockDevInfo(id, reload);
    }
}

QVariantMap DeviceProxyManager::queryProtocolInfo(const QString &id, bool reload)
{
    if (d->isDBusRuning()) {
        auto &&reply = d->devMngDBus->QueryProtocolDeviceInfo(id, reload);
        reply.waitForFinished();
        return reply.value();
    } else {
        return DevMngIns->getProtocolDevInfo(id, reload);
    }
}

void DeviceProxyManager::detachBlockDevice(const QString &id)
{
    if (d->isDBusRuning())
        d->devMngDBus->DetachBlockDevice(id);
    else
        DevMngIns->detachBlockDev(id);
}

void DeviceProxyManager::detachProtocolDevice(const QString &id)
{
    if (d->isDBusRuning())
        d->devMngDBus->DetachProtocolDevice(id);
    else
        DevMngIns->detachProtoDev(id);
}

void DeviceProxyManager::detachAllDevices()
{
    if (d->isDBusRuning()) {
        d->devMngDBus->DetachAllMountedDevices();
    } else {
        DevMngIns->detachAllRemovableBlockDevs();
        DevMngIns->detachAllProtoDevs();
    }
}

void DeviceProxyManager::reloadOpticalInfo(const QString &id)
{
    if (d->isDBusRuning())
        queryBlockInfo(id, true);
    else
        DevMngIns->getBlockDevInfo(id, true);
}

bool DeviceProxyManager::connectToService()
{
    qInfo() << "Start initilize dbus: `DeviceManagerInterface`";
    d->devMngDBus.reset(new DeviceManagerInterface(kDesktopService, kDevMngPath, QDBusConnection::sessionBus(), this));
    d->initConnection();
    return d->isDBusRuning();
}

bool DeviceProxyManager::isMonitorWorking()
{
    auto &&reply = d->devMngDBus->IsMonotorWorking();
    reply.waitForFinished();
    return reply.value();
}

bool DeviceProxyManager::isDBusRuning()
{
    return d->isDBusRuning();
}

bool DeviceProxyManager::isFileOfExternalMounts(const QString &filePath)
{
    const QStringList &&mpts = d->externalMounts.values();
    QString path = filePath.endsWith("/") ? filePath : filePath + "/";
    auto ret = std::find_if(mpts.cbegin(), mpts.cend(), [path](const QString &mpt) { return path.startsWith(mpt); });
    return ret != mpts.cend();
}

bool DeviceProxyManager::isFileOfProtocolMounts(const QString &filePath)
{
    QString path = filePath.endsWith("/") ? filePath : filePath + "/";
    for (auto iter = d->allMounts.constKeyValueBegin(); iter != d->allMounts.constKeyValueEnd(); ++iter) {
        if (!iter.base().key().startsWith(kBlockDeviceIdPrefix) && path.startsWith(iter.base().value()))
            return true;
    }
    return false;
}

bool DeviceProxyManager::isFileFromOptical(const QString &filePath)
{
    QString path = filePath.endsWith("/") ? filePath : filePath + "/";
    for (auto iter = d->allMounts.constKeyValueBegin(); iter != d->allMounts.constKeyValueEnd(); ++iter) {
        if (iter.base().key().startsWith(QString(kBlockDeviceIdPrefix) + "sr") && path.startsWith(iter.base().value()))
            return true;
    }
    return false;
}

bool DeviceProxyManager::isMptOfDevice(const QString &filePath, QString &id)
{
    QString path = filePath.endsWith("/") ? filePath : filePath + "/";
    id = d->allMounts.key(path, "");
    return !id.isEmpty();
}

DeviceProxyManager::DeviceProxyManager(QObject *parent)
    : QObject(parent), d(new DeviceProxyManagerPrivate(this, parent))
{
    QTimer::singleShot(1000, this, [this] { d->initMounts(); });
}

DeviceProxyManager::~DeviceProxyManager()
{
}

DeviceProxyManagerPrivate::DeviceProxyManagerPrivate(DeviceProxyManager *qq, QObject *parent)
    : QObject(parent), q(qq)
{
}

DeviceProxyManagerPrivate::~DeviceProxyManagerPrivate()
{
}

bool DeviceProxyManagerPrivate::isDBusRuning()
{
    return devMngDBus && devMngDBus->isValid();
}

void DeviceProxyManagerPrivate::initConnection()
{
    dbusWatcher.reset(new QDBusServiceWatcher(kDesktopService, QDBusConnection::sessionBus()));
    q->connect(dbusWatcher.data(), &QDBusServiceWatcher::serviceRegistered, q, [this] {
        connectToDBus();
        emit q->devMngDBusRegistered();
    });
    q->connect(dbusWatcher.data(), &QDBusServiceWatcher::serviceUnregistered, q, [this] {
        connectToAPI();
        emit q->devMngDBusUnregistered();
    });

    if (isDBusRuning())
        connectToDBus();
    else
        connectToAPI();
}

void DeviceProxyManagerPrivate::initMounts()
{
    using namespace GlobalServerDefines;

    auto func = [this](const QStringList &devs, std::function<QVariantMap(DeviceProxyManager *, const QString &, bool)> query) {
        for (const auto &dev : devs) {
            auto &&info = query(q, dev, false);
            auto mpt = info.value(DeviceProperty::kMountPoint).toString();
            if (!mpt.isEmpty()) {
                mpt = mpt.endsWith("/") ? mpt : mpt + "/";
                if (info.value(DeviceProperty::kRemovable).toBool())
                    externalMounts.insert(dev, mpt);
                allMounts.insert(dev, mpt);
            }
        }
    };

    auto blks = q->getAllBlockIds();
    auto protos = q->getAllProtocolIds();
    func(blks, &DeviceProxyManager::queryBlockInfo);
    func(protos, &DeviceProxyManager::queryProtocolInfo);
}

void DeviceProxyManagerPrivate::connectToDBus()
{
    if (currentConnectionType == kDBusConnecting)
        return;
    disconnCurrentConnections();

    auto ptr = devMngDBus.data();
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDriveAdded, q, &DeviceProxyManager::blockDriveAdded);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDriveRemoved, q, &DeviceProxyManager::blockDriveRemoved);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceAdded, q, &DeviceProxyManager::blockDevAdded);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceRemoved, q, &DeviceProxyManager::blockDevRemoved);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceMounted, q, &DeviceProxyManager::blockDevMounted);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceUnmounted, q, &DeviceProxyManager::blockDevUnmounted);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceLocked, q, &DeviceProxyManager::blockDevLocked);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceUnlocked, q, &DeviceProxyManager::blockDevUnlocked);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceFilesystemAdded, q, &DeviceProxyManager::blockDevFsAdded);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceFilesystemRemoved, q, &DeviceProxyManager::blockDevFsRemoved);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDevicePropertyChanged, q, [this](const QString &id, const QString &property, const QDBusVariant &value) {
        emit this->q->blockDevPropertyChanged(id, property, value.variant());
    });

    connections << q->connect(ptr, &DeviceManagerInterface::SizeUsedChanged, q, &DeviceProxyManager::devSizeChanged);

    connections << q->connect(ptr, &DeviceManagerInterface::ProtocolDeviceAdded, q, &DeviceProxyManager::protocolDevAdded);
    connections << q->connect(ptr, &DeviceManagerInterface::ProtocolDeviceRemoved, q, &DeviceProxyManager::protocolDevRemoved);
    connections << q->connect(ptr, &DeviceManagerInterface::ProtocolDeviceMounted, q, &DeviceProxyManager::protocolDevMounted);
    connections << q->connect(ptr, &DeviceManagerInterface::ProtocolDeviceUnmounted, q, &DeviceProxyManager::protocolDevUnmounted);

    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceRemoved, this, &DeviceProxyManagerPrivate::removeMounts);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceMounted, this, &DeviceProxyManagerPrivate::addMounts);
    connections << q->connect(ptr, &DeviceManagerInterface::BlockDeviceUnmounted, this, &DeviceProxyManagerPrivate::removeMounts);
    connections << q->connect(ptr, &DeviceManagerInterface::ProtocolDeviceRemoved, this, &DeviceProxyManagerPrivate::removeMounts);
    connections << q->connect(ptr, &DeviceManagerInterface::ProtocolDeviceMounted, this, &DeviceProxyManagerPrivate::addMounts);
    connections << q->connect(ptr, &DeviceManagerInterface::ProtocolDeviceUnmounted, this, &DeviceProxyManagerPrivate::removeMounts);

    currentConnectionType = kDBusConnecting;
}

void DeviceProxyManagerPrivate::connectToAPI()
{
    if (currentConnectionType == kAPIConnecting)
        return;
    disconnCurrentConnections();

    auto ptr = DevMngIns;
    connections << q->connect(ptr, &DeviceManager::blockDriveAdded, q, &DeviceProxyManager::blockDriveAdded);
    connections << q->connect(ptr, &DeviceManager::blockDriveRemoved, q, &DeviceProxyManager::blockDriveRemoved);
    connections << q->connect(ptr, &DeviceManager::blockDevAdded, q, &DeviceProxyManager::blockDevAdded);
    connections << q->connect(ptr, &DeviceManager::blockDevRemoved, q, &DeviceProxyManager::blockDevRemoved);
    connections << q->connect(ptr, &DeviceManager::blockDevMounted, q, &DeviceProxyManager::blockDevMounted);
    connections << q->connect(ptr, &DeviceManager::blockDevUnmounted, q, &DeviceProxyManager::blockDevUnmounted);
    connections << q->connect(ptr, &DeviceManager::blockDevLocked, q, &DeviceProxyManager::blockDevLocked);
    connections << q->connect(ptr, &DeviceManager::blockDevUnlocked, q, &DeviceProxyManager::blockDevUnlocked);
    connections << q->connect(ptr, &DeviceManager::blockDevFsAdded, q, &DeviceProxyManager::blockDevFsAdded);
    connections << q->connect(ptr, &DeviceManager::blockDevFsRemoved, q, &DeviceProxyManager::blockDevFsRemoved);
    connections << q->connect(ptr, &DeviceManager::blockDevPropertyChanged, q, &DeviceProxyManager::blockDevPropertyChanged);

    connections << q->connect(ptr, &DeviceManager::devSizeChanged, q, &DeviceProxyManager::devSizeChanged);

    connections << q->connect(ptr, &DeviceManager::protocolDevAdded, q, &DeviceProxyManager::protocolDevAdded);
    connections << q->connect(ptr, &DeviceManager::protocolDevRemoved, q, &DeviceProxyManager::protocolDevRemoved);
    connections << q->connect(ptr, &DeviceManager::protocolDevMounted, q, &DeviceProxyManager::protocolDevMounted);
    connections << q->connect(ptr, &DeviceManager::protocolDevUnmounted, q, &DeviceProxyManager::protocolDevUnmounted);

    connections << q->connect(ptr, &DeviceManager::blockDevRemoved, this, &DeviceProxyManagerPrivate::removeMounts);
    connections << q->connect(ptr, &DeviceManager::blockDevMounted, this, &DeviceProxyManagerPrivate::addMounts);
    connections << q->connect(ptr, &DeviceManager::blockDevUnmounted, this, &DeviceProxyManagerPrivate::removeMounts);
    connections << q->connect(ptr, &DeviceManager::protocolDevRemoved, this, &DeviceProxyManagerPrivate::removeMounts);
    connections << q->connect(ptr, &DeviceManager::protocolDevMounted, this, &DeviceProxyManagerPrivate::addMounts);
    connections << q->connect(ptr, &DeviceManager::protocolDevUnmounted, this, &DeviceProxyManagerPrivate::removeMounts);

    currentConnectionType = kAPIConnecting;

    DevMngIns->startMonitor();
}

void DeviceProxyManagerPrivate::disconnCurrentConnections()
{
    for (const auto &connection : connections)
        q->disconnect(connection);
    connections.clear();
    currentConnectionType = kNoneConnection;
}

void DeviceProxyManagerPrivate::addMounts(const QString &id, const QString &mpt)
{
    QString p = mpt.endsWith("/") ? mpt : mpt + "/";
    if (id.startsWith(kBlockDeviceIdPrefix)) {
        auto &&info = q->queryBlockInfo(id);
        if (info.value(GlobalServerDefines::DeviceProperty::kRemovable).toBool())
            externalMounts.insert(id, p);
    } else {
        externalMounts.insert(id, p);
    }
    allMounts.insert(id, p);
}

void DeviceProxyManagerPrivate::removeMounts(const QString &id)
{
    externalMounts.remove(id);
    allMounts.remove(id);
}