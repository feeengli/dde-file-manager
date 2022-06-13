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
#ifndef COLLECTIONHOLDER_H
#define COLLECTIONHOLDER_H

#include "ddplugin_organizer_global.h"

#include <QObject>

DDP_ORGANIZER_BEGIN_NAMESPACE

class FileProxyModel;
class CollectionView;
class CollectionHolderPrivate;
class CollectionHolder : public QObject
{
    Q_OBJECT
    friend class CollectionHolderPrivate;
public:
    explicit CollectionHolder(const QString &uuid, QObject *parent = nullptr);
    QString id() const;
    QString name();
    void setName(const QString &);
    QList<QUrl> urls() const;
    void setUrls(const QList<QUrl> &urls);
    void createView(FileProxyModel *model);

signals:

public slots:
private:
    CollectionHolderPrivate *d;
};

typedef QSharedPointer<CollectionHolder> CollectionHolderPointer;

DDP_ORGANIZER_END_NAMESPACE

#endif // COLLECTIONHOLDER_H