/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangsheng<zhangsheng@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             xushitong<xushitong@uniontech.com>
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
#include "filedialogmanagerdbus.h"
#include "dbus/filedialoghandledbus.h"
#include "dbus/filedialog_adaptor.h"
#include "utils/appexitcontroller.h"

#include "dfm-base/base/application/application.h"
#include "dfm-base/base/application/settings.h"
#include "dfm-base/mimetype/dmimedatabase.h"

#include <dfm-framework/event/event.h>

#include <QApplication>
#include <QDBusConnection>
#include <QUuid>

DFMBASE_USE_NAMESPACE

FileDialogManagerDBus::FileDialogManagerDBus(QObject *parent)
    : QObject(parent)
{
    connect(qApp, &QApplication::lastWindowClosed, this, [this]() {
        lastWindowClosed = true;
        onAppExit();
    });
}

QDBusObjectPath FileDialogManagerDBus::createDialog(QString key)
{
    if (key.isEmpty())
        key = QUuid::createUuid().toRfc4122().toHex();

    FileDialogHandleDBus *handle = new FileDialogHandleDBus();
    Q_UNUSED(new FiledialogAdaptor(handle));

    const QDBusObjectPath path("/com/deepin/filemanager/filedialog/" + key);

    if (curDialogObjectMap.contains(path)) {
        return path;
    }

    if (!QDBusConnection::sessionBus().registerObject(path.path(), handle)) {
        qWarning("Cannot register to the D-Bus object.\n");
        handle->deleteLater();

        return QDBusObjectPath();
    }

    curDialogObjectMap[path] = handle;
    connect(handle, &FileDialogHandleDBus::destroyed, this, &FileDialogManagerDBus::onDialogDestroy);
    DIALOGCORE_NAMESPACE::AppExitController::instance().dismiss();
    return path;
}

void FileDialogManagerDBus::destroyDialog(const QDBusObjectPath &path)
{
    QObject *object = curDialogObjectMap.value(path);

    if (object)
        object->deleteLater();
}

QList<QDBusObjectPath> FileDialogManagerDBus::dialogs() const
{
    return curDialogObjectMap.keys();
}

/*!
 * \brief deprecated: Just compatibility
 */
QString FileDialogManagerDBus::errorString() const
{
    return {};
}

bool FileDialogManagerDBus::isUseFileChooserDialog() const
{
    return Application::instance()->genericAttribute(Application::kOverrideFileChooserDialog).toBool();
}

bool FileDialogManagerDBus::canUseFileChooserDialog(const QString &group, const QString &executableFileName) const
{
    const QString &groupName = QStringLiteral("DBusFileDialog");
    const QVariantMap &blackMap = Application::appObtuselySetting()->value(groupName, "disable").toMap();

    return !blackMap.value(group).toStringList().contains(executableFileName);
}

QStringList FileDialogManagerDBus::globPatternsForMime(const QString &mimeType) const
{
    DFMBASE_NAMESPACE::DMimeDatabase db;
    QMimeType mime(db.mimeTypeForName(mimeType));
    if (mime.isValid()) {
        if (mime.isDefault()) {
            return QStringList(QStringLiteral("*"));
        } else {
            return mime.globPatterns();
        }
    }
    return QStringList();
}

/*!
 * \brief deprecated: Just compatibility
 */
QStringList FileDialogManagerDBus::monitorFiles() const
{
    return {};
}

void FileDialogManagerDBus::showBluetoothTransDialog(const QString &id, const QStringList &URIs)
{
    dpfSlotChannel->push("dfmplugin_utils", "slot_Bluetooth_SendFiles", URIs, id);
}

void FileDialogManagerDBus::onDialogDestroy()
{
    const QDBusObjectPath &path = curDialogObjectMap.key(QObject::sender());

    if (!path.path().isEmpty())
        curDialogObjectMap.remove(path);

    onAppExit();
}

void FileDialogManagerDBus::onAppExit()
{
    if (lastWindowClosed && curDialogObjectMap.size() == 0) {
        // after 1 minutes app exit if don't use filedialog
        DIALOGCORE_NAMESPACE::AppExitController::instance().readyToExit(60, [this]() {
            // last confirm exit
            if (lastWindowClosed && curDialogObjectMap.size() == 0)
                return true;
            return false;
        });
    }
}