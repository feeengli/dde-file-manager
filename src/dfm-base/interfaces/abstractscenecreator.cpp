/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangyu<zhangyub@uniontech.com>
 *
 * Maintainer: liqiang<liqianga@uniontech.com>
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
#include "abstractscenecreator.h"

using namespace dfmbase;

AbstractSceneCreator::AbstractSceneCreator()
{
}

AbstractSceneCreator::~AbstractSceneCreator()
{
}

bool AbstractSceneCreator::addChild(const QString &scene)
{
    if (scene.isEmpty())
        return false;

    if (!children.contains(scene))
        children.append(scene);
    return true;
}

void AbstractSceneCreator::removeChild(const QString &scene)
{
    children.removeOne(scene);
}