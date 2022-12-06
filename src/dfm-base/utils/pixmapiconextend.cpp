/*
 * Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuyangming<liuyangming@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
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
#include "pixmapiconextend.h"
#include <private/qicon_p.h>
#include <QApplication>

PixmapIconExtend::PixmapIconExtend(const QIcon &other)
    : QIcon(other)
{
}

PixmapIconExtend::~PixmapIconExtend()
{
}

QPixmap PixmapIconExtend::pixmapExtend(const QSize &size, qreal devicePixelRatio, QIcon::Mode mode, QIcon::State state)
{
    DataPtr d = data_ptr();
    if (!d)
        return QPixmap();

    if (devicePixelRatio <= -1.0)
        devicePixelRatio = qApp->devicePixelRatio();

    if (devicePixelRatio > 1.0) {
        QPixmap pixmap = d->engine->pixmap(size, mode, state);
        pixmap.setDevicePixelRatio(1.0);
    }

    QPixmap pixmap = d->engine->scaledPixmap(size * devicePixelRatio, mode, state, devicePixelRatio);
    pixmap.setDevicePixelRatio(pixmapDevicePixelRatio(devicePixelRatio, size, pixmap.size()));
    return pixmap;
}

qreal PixmapIconExtend::pixmapDevicePixelRatio(qreal displayDevicePixelRatio, const QSize &requestedSize, const QSize &actualSize)
{
    QSize targetSize = requestedSize * displayDevicePixelRatio;
    qreal scale = 0.5 * (qreal(actualSize.width()) / qreal(targetSize.width()) + qreal(actualSize.height() / qreal(targetSize.height())));
    return qMax(qreal(1.0), displayDevicePixelRatio * scale);
}