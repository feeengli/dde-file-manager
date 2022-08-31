/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lixiang<lixianga@uniontech.com>
 *
 * Maintainer: lixiang<lixianga@uniontech.com>
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
#include "servicemanager.h"

#include <mutex>
#include <QUrl>

using namespace dfmplugin_vault;
ServiceManager::ServiceManager(QObject *parent)
    : QObject(parent)
{
}

ServiceManager::ExpandFieldMap ServiceManager::basicViewFieldFunc(const QUrl &url)
{
    BasicExpand expandFiledMap;
    expandFiledMap.insert("kFilePosition", qMakePair(tr("Location"), url.url()));

    ExpandFieldMap expandMap;
    expandMap.insert("kFieldReplace", expandFiledMap);
    return expandMap;
}