/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
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
#ifndef DETAILSPACEEVENTRECEIVER_H
#define DETAILSPACEEVENTRECEIVER_H

#include "dfmplugin_detailspace_global.h"

#include <QObject>
#include <QItemSelection>

namespace dfmplugin_detailspace {

class DetailSpaceEventReceiver final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DetailSpaceEventReceiver)

public:
    static DetailSpaceEventReceiver &instance();

    void connectService();

public slots:
    void handleTileBarShowDetailView(quint64 windowId, bool checked);
    void handleSetSelect(quint64 windowId, const QUrl &url);
    bool handleViewExtensionRegister(CustomViewExtensionView view, int index);
    void handleViewExtensionUnregister(int index);
    bool handleBasicViewExtensionRegister(const QString &scheme, BasicViewFieldFunc func);
    void handleBasicViewExtensionUnregister(const QString &scheme);
    bool handleBasicFiledFilterAdd(const QString &scheme, const QStringList &enums);
    void handleBasicFiledFilterRemove(const QString &scheme);

    // worksapce
    void handleViewSelectionChanged(const quint64 windowID, const QItemSelection &selected, const QItemSelection &deselected);

private:
    explicit DetailSpaceEventReceiver(QObject *parent = nullptr);
};

}

#endif   // DETAILSPACEEVENTRECEIVER_H