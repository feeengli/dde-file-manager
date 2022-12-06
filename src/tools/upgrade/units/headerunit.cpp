/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangyu<zhangyub@uniontech.com>
 *
 * Maintainer: zhangyu<zhangyub@uniontech.com>
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
#include "headerunit.h"

#include <QDebug>

using namespace dfm_upgrade;

HeaderUnit::HeaderUnit() : UpgradeUnit()
{

}

QString HeaderUnit::name()
{
    return "Header";
}

bool HeaderUnit::initialize(const QMap<QString, QString> &args)
{
    qInfo() << "begin upgrade. the args are" << args;
    time.start();
    return true;
}

bool HeaderUnit::upgrade()
{
    qInfo() << "init all units spend:" << time.elapsed();
    time.start();
    return true;
}

void HeaderUnit::completed()
{
    qInfo() << "all units upgraded and spended:" << time.elapsed();
}