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
#ifndef CONFIGCENTER_H
#define CONFIGCENTER_H

#include "dfm_base_global.h"

#include <QObject>

DFMBASE_BEGIN_NAMESPACE

enum ConfigType {
    kDConfig,
    kGSetting
};

class ConfigCenter : public QObject
{
    Q_OBJECT
public:
    static ConfigCenter *instance();

private:
    explicit ConfigCenter(QObject *parent = nullptr);
    ~ConfigCenter() override;

signals:
};

DFMBASE_END_NAMESPACE

#endif   // CONFIGCENTER_H