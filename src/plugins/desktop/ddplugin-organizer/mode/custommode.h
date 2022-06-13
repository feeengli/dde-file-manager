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
#ifndef CUSTOMMODE_H
#define CUSTOMMODE_H

#include "canvasorganizer.h"

DDP_ORGANIZER_BEGIN_NAMESPACE

class CustomModePrivate;
class CustomMode : public CanvasOrganizer
{
    Q_OBJECT
    friend class CustomModePrivate;
public:
    explicit CustomMode(QObject *parent = nullptr);
    int mode() const override;
    bool initialize(FileProxyModel *) override;
private:
    CustomModePrivate *d;
};

DDP_ORGANIZER_END_NAMESPACE

#endif // CUSTOMMODE_H