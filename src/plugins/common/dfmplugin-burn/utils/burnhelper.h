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
#ifndef BURNHELPER_H
#define BURNHELPER_H

#include "dfmplugin_burn_global.h"

#include <QUrl>

DPBURN_BEGIN_NAMESPACE

class BurnHelper
{
public:
    static int showOpticalBlankConfirmationDialog();
    static int showOpticalImageOpSelectionDialog();
    static QUrl localStagingFile(QString dev);
    static QUrl localStagingFile(const QUrl &dest);
    static QString parseXorrisoErrorMessage(const QStringList &msg);
    static QString burnDestDevice(const QUrl &url);
    static QString burnFilePath(const QUrl &url);
};

DPBURN_END_NAMESPACE

#endif   // BURNHELPER_H