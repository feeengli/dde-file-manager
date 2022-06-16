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
#ifndef OPENWITHEVENTRECEIVER_H
#define OPENWITHEVENTRECEIVER_H
#include "dfmplugin_utils_global.h"

#include <QObject>

DPUTILS_BEGIN_NAMESPACE
class OpenWithEventReceiver : public QObject
{
    Q_OBJECT
public:
    static OpenWithEventReceiver *instance();

    void initEventConnect();

public:   //! slot event
    void showOpenWithDialog(const QList<QUrl> &urls);

private:
    explicit OpenWithEventReceiver(QObject *parent = nullptr);
};
DPUTILS_END_NAMESPACE
#endif   // OPENWITHEVENTRECEIVER_H