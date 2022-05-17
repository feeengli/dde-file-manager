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
#ifndef DEVICEUTILS_H
#define DEVICEUTILS_H

#include "dfm-base/dfm_base_global.h"

#include <QString>

#include <dfm-mount/base/dmountutils.h>

DFMBASE_BEGIN_NAMESPACE

namespace BlockAdditionalProperty {
static constexpr char kClearBlockProperty[] { "ClearBlockDeviceInfo" };
static constexpr char kAliasGroupName[] { "LocalDiskAlias" };
static constexpr char kAliasItemName[] { "Items" };
static constexpr char kAliasItemUUID[] { "uuid" };
static constexpr char kAliasItemAlias[] { "alias" };
}   // namespace BlockAdditionalProperty

static constexpr char kBlockDeviceIdPrefix[] { "/org/freedesktop/UDisks2/block_devices/" };

/*!
 * \brief The DeviceUtils class
 * this class provide some util functions.
 */
class DeviceUtils
{
public:
    static QString getBlockDeviceId(const QString &deviceDesc);
    static QString errMessage(DFMMOUNT::DeviceError err);
    static QString convertSuitableDisplayName(const QVariantMap &devInfo);
    static QString convertSuitableDisplayName(const QVariantHash &devInfo);
    static bool isAutoMountEnable();
    static bool isAutoMountAndOpenEnable();

private:
    static QString nameOfSystemDisk(const QVariantMap &datas);
    static QString nameOfOptical(const QVariantMap &datas);
    static QString nameOfEncrypted(const QVariantMap &datas);
    static QString nameOfDefault(const QString &label, const quint64 &size);
    static QString nameOfSize(const quint64 &size);
};

DFMBASE_END_NAMESPACE

#endif   // DEVICEUTILS_H