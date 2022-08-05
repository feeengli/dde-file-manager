﻿/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
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
#ifndef BOXSELECTER_H
#define BOXSELECTER_H

#include "ddplugin_canvas_global.h"

#include <QRect>
#include <QPoint>
#include <QObject>

class QItemSelection;

namespace ddplugin_canvas {
class CanvasView;
class BoxSelecter : public QObject
{
    Q_OBJECT
public:
    static BoxSelecter *instance();
    void beginSelect(const QPoint &globalPos, bool autoSelect);
    void endSelect();
    void setBegin(const QPoint &globalPos);
    void setEnd(const QPoint &globalPos);
    QRect validRect(CanvasView *) const;
    QRect globalRect() const;
    QRect clipRect(QRect rect, const QRect &geometry) const;
    bool isBeginFrom(CanvasView *w);
    void selection(CanvasView *w, const QRect &rect, QItemSelection *newSelection);
public:
    inline bool isAcvite() const {
        return active;
    }
protected:
    void setAcvite(bool ac);
    explicit BoxSelecter(QObject *parent = nullptr);
    bool eventFilter(QObject *watched, QEvent *event);
protected:
    virtual void updateSelection();
    virtual void updateCurrentIndex();
private:
    void selection(QItemSelection *newSelection);
signals:
    void changed();
public slots:
private:
    bool automatic = false;
    bool active = false;
    QPoint begin;
    QPoint end;
};

#define BoxSelIns DDP_CANVAS_NAMESPACE::BoxSelecter::instance()

}

#endif // BOXSELECTER_H