/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     wangchunlin<wangchunlin@uniontech.com>
 *
 * Maintainer: wangchunlin<wangchunlin@uniontech.com>
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
#ifndef CANVASGRIDSHELL_H
#define CANVASGRIDSHELL_H

#include <QObject>

namespace ddplugin_organizer {

class CanvasGridShell : public QObject
{
    Q_OBJECT
public:
    explicit CanvasGridShell(QObject *parent = nullptr);
    ~CanvasGridShell();
    bool initialize();

public:
    QString item(int index, const QPoint &gridPos);
    void tryAppendAfter(const QStringList &items, int index, const QPoint &begin);
    int point(const QString &item, QPoint *pos);
};

}

#endif // CANVASGRIDSHELL_H