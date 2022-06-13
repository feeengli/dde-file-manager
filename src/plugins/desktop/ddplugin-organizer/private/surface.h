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
#ifndef SURFACE_H
#define SURFACE_H

#include "ddplugin_organizer_global.h"

#include <QWidget>

DDP_ORGANIZER_BEGIN_NAMESPACE

class Surface : public QWidget
{
    Q_OBJECT
public:
    explicit Surface(QWidget *parent = nullptr);

signals:

public slots:
protected:
#ifdef QT_DEBUG
    void paintEvent(QPaintEvent *) override;
#endif
};

typedef QSharedPointer<Surface> SurfacePointer;

DDP_ORGANIZER_END_NAMESPACE

#endif // SURFACE_H