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
#ifndef CANVASPROXYMODEL_P_H
#define CANVASPROXYMODEL_P_H

#include "canvasproxymodel.h"
#include "fileinfomodel.h"
#include "modelextendinterface.h"
#include "canvasmodelfilter.h"

#include <QTimer>

DDP_CANVAS_BEGIN_NAMESPACE

class CanvasProxyModelPrivate : public QObject
{
    Q_OBJECT
public:
    explicit CanvasProxyModelPrivate(CanvasProxyModel *qq);
    void clearMapping();
    void createMapping();
    QModelIndexList indexs() const;
    bool doSort(QList<DFMLocalFileInfoPointer> &files) const;
public slots:
    void doRefresh(bool global);
    void sourceDataChanged(const QModelIndex &sourceTopleft,
                              const QModelIndex &sourceBottomright,
                              const QVector<int> &roles);
    void sourceAboutToBeReset();
    void sourceReset();

    void sourceRowsInserted(const QModelIndex &sourceParent,
                               int start, int end);
    void sourceRowsAboutToBeRemoved(const QModelIndex &sourceParent,
                                       int start, int end);
    void sourceDataRenamed(const QUrl &oldUrl, const QUrl &newUrl);
protected:
    void specialSort(QList<DFMLocalFileInfoPointer> &files) const;
    void sortMainDesktopFile(QList<DFMLocalFileInfoPointer> &files, Qt::SortOrder order) const;
    bool insertFilter(const QUrl &url);
    bool resetFilter(QList<QUrl> &urls);
    bool updateFilter(const QUrl &url);
    bool removeFilter(const QUrl &url);
    bool renameFilter(const QUrl &oldUrl, const QUrl &newUrl);

public:
    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System;
    QList<QUrl> fileList;
    QMap<QUrl, DFMLocalFileInfoPointer> fileMap;
    FileInfoModel *srcModel = nullptr;
    QSharedPointer<QTimer> refreshTimer;
    int fileSortRole = DFMBASE_NAMESPACE::AbstractFileInfo::kSortByFileName;
    Qt::SortOrder fileSortOrder = Qt::AscendingOrder;
    ModelExtendInterface *extend = nullptr;
    QList<QSharedPointer<CanvasModelFilter>> modelFilters;
private:
    CanvasProxyModel *q;
};


DDP_CANVAS_END_NAMESPACE

#endif // CANVASPROXYMODEL_P_H