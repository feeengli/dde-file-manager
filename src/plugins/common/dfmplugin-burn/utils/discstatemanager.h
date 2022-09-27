/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangsheng<zhangsheng@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             xushitong<xushitong@uniontech.com>
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
#ifndef DISCSTATEMANAGER_H
#define DISCSTATEMANAGER_H

#include "dfmplugin_burn_global.h"

#include <QObject>

namespace dfmplugin_burn {

class DiscStateManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DiscStateManager)

public:
    static DiscStateManager *instance();

    void initilaize();

private slots:
    void ghostMountForBlankDisc();
    void onDevicePropertyChanged(const QString &id, const QString &propertyName, const QVariant &var);

private:
    explicit DiscStateManager(QObject *parent = nullptr);
};

}

#endif   // DISCSTATEMANAGER_H