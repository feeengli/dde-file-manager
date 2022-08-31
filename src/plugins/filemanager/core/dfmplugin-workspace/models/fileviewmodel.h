/*
 * Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huanyu<huanyub@uniontech.com>
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

#ifndef FILEVIEWMODEL_H
#define FILEVIEWMODEL_H

#include "views/fileviewitem.h"

#include "dfm-base/file/local/localfileinfo.h"
#include "dfm-base/base/schemefactory.h"
#include "dfm-base/dfm_global_defines.h"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QUrl>

#include <iostream>
#include <memory>

#include <unistd.h>

class QAbstractItemView;

namespace dfmplugin_workspace {

class FileViewModelPrivate;
class FileViewModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class FileViewModelPrivate;
    QSharedPointer<FileViewModelPrivate> d;

public:
    enum State {
        Idle,
        Busy
    };

    explicit FileViewModel(QAbstractItemView *parent = nullptr);
    virtual ~FileViewModel() override;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual void fetchMore(const QModelIndex &parent) override;
    virtual bool canFetchMore(const QModelIndex &parent) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QStringList mimeTypes() const override;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    virtual Qt::DropActions supportedDragActions() const override;
    virtual Qt::DropActions supportedDropActions() const override;

    void beginInsertRows(const QModelIndex &parent, int first, int last);
    void endInsertRows();

    void beginRemoveRows(const QModelIndex &parent, int first, int last);
    void endRemoveRows();

    QUrl rootUrl() const;
    QModelIndex rootIndex() const;
    const FileViewItem *rootItem() const;
    void setRootUrl(const QUrl &url);

    void clear();
    void update();

    AbstractFileWatcherPointer fileWatcher() const;
    const FileViewItem *itemFromIndex(const QModelIndex &index) const;
    AbstractFileInfoPointer fileInfo(const QModelIndex &index) const;

    QModelIndex findIndex(const QUrl &url) const;

    State state() const;
    void setState(FileViewModel::State state);
    void childrenUpdated();

    void traversCurrDir();
    void stopTraversWork();

    void selectAndRenameFile(const QUrl &fileUrl);

    QList<DFMGLOBAL_NAMESPACE::ItemRoles> getColumnRoles() const;
public slots:
    void onFilesUpdated();
    void onFileUpdated(const QUrl &url);

signals:
    void stateChanged();
    void modelChildrenUpdated();
    void updateFiles();
    void selectAndEditFile(const QUrl &url);
};

}

#endif   // FILEVIEWMODEL_H