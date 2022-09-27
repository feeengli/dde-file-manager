/*
 * Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co., Ltd.
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
#include "sidebarmodel.h"
#include "sidebaritemdelegate.h"
#include "sidebaritem.h"
#include "utils/sidebarhelper.h"
#include "utils/sidebarinfocachemananger.h"

#include "dfm-base/utils/universalutils.h"
#include <dfm-framework/event/event.h>

#include <QMimeData>
#include <QDebug>
#include <QtConcurrent>

static constexpr char kModelitemMimetype[] { "application/x-dfmsidebaritemmodeldata" };

DPSIDEBAR_USE_NAMESPACE

namespace GlobalPrivate {
QByteArray generateMimeData(const QModelIndexList &indexes)
{
    if (indexes.isEmpty())
        return QByteArray();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    stream << indexes.first().row();

    return encoded;
}

int getRowIndexFromMimeData(const QByteArray &data)
{
    int row;
    QDataStream stream(data);
    stream >> row;

    return row;
}
}   // namespace GlobalPrivate

/*!
 * \class SideBarModel
 * \brief
 */
SideBarModel::SideBarModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

bool SideBarModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    // when drag onto the empty space of the area, just return false.
    if (column == -1 || row == -1 || !data)
        return false;

    Q_ASSERT(column == 0);

    auto isSeparator = [](SideBarItem *item) -> bool {
        return item && dynamic_cast<SideBarItemSeparator *>(item);
    };
    auto isItemDragEnabled = [](SideBarItem *item) -> bool {
        return item && item->flags().testFlag(Qt::ItemIsDragEnabled);
    };
    auto isTheSameGroup = [](SideBarItem *item1, SideBarItem *item2) -> bool {
        return item1 && item2 && item1->group() == item2->group();
    };

    SideBarItem *targetItem = this->itemFromIndex(row, parent);
    SideBarItem *sourceItem = nullptr;

    if (isSeparator(targetItem))   //According to the requirement，sparator does not support to drop.
        return false;

    // check if is item internal move by action and mimetype:
    if (action == Qt::MoveAction && data->formats().contains(kModelitemMimetype)) {
        int oriRowIndex = GlobalPrivate::getRowIndexFromMimeData(data->data(kModelitemMimetype));
        if (oriRowIndex >= 0) {
            sourceItem = this->itemFromIndex(oriRowIndex, parent);
        }

        // normal drag tag or bookmark or quick access
        if (isItemDragEnabled(targetItem) && isTheSameGroup(sourceItem, targetItem)) {
            return true;
        }

        SideBarItem *prevItem = itemFromIndex(row - 1, parent);
        // drag tag item to bottom, targetItem is null
        // drag bookmark item on the bookmark bottom separator, targetItem is Separator
        if ((!targetItem || isSeparator(targetItem)) && sourceItem != prevItem) {
            return isItemDragEnabled(prevItem) && isTheSameGroup(prevItem, sourceItem);
        }

        return false;
    }

    return QStandardItemModel::canDropMimeData(data, action, row, column, parent);
}

QMimeData *SideBarModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *data = QStandardItemModel::mimeData(indexes);
    if (!data)
        return nullptr;
    data->setData(kModelitemMimetype, GlobalPrivate::generateMimeData(indexes));
    return data;
}

SideBarItem *SideBarModel::itemFromIndex(const QModelIndex &index) const
{
    QStandardItem *item = QStandardItemModel::itemFromIndex(index);
    SideBarItem *castedItem = static_cast<SideBarItem *>(item);

    return castedItem;
}

SideBarItem *SideBarModel::itemFromIndex(int index, const QModelIndex &parent) const
{
    return itemFromIndex(this->index(index, 0, parent));
}

bool SideBarModel::insertRow(int row, SideBarItem *item)
{
    if (!item)
        return false;

    if (0 > row)
        return false;

    if (findRowByUrl(item->url()).row() > 0)
        return true;

    SideBarItemSeparator *groupItem = dynamic_cast<SideBarItemSeparator *>(item);
    if (groupItem) {   //top item
        QStandardItemModel::insertRow(row + 1, item);   //insert the top item
        return rowCount() - 1;
    } else {   //sub item
        int count = this->rowCount();
        for (int i = 0; i < count; i++) {
            const QModelIndex &index = this->index(i, 0);
            if (!index.isValid())
                continue;
            if (index.data(SideBarItem::Roles::kItemGroupRole).toString() != item->group())
                continue;
            SideBarItem *groupItem = this->itemFromIndex(index);
            if (groupItem) {
                int rows = groupItem->rowCount();
                if (row >= 0 && row < rows)
                    groupItem->insertRow(row, item);
                else
                    groupItem->insertRow(0, item);
            }
            return true;
        }
    }

    return true;
}

int SideBarModel::appendRow(SideBarItem *item)
{
    if (!item)
        return -1;

    auto r = findRowByUrl(item->url()).row();
    if (r > 0)
        return r;

    SideBarItemSeparator *topItem = dynamic_cast<SideBarItemSeparator *>(item);
    SideBarItem *groupOther = nullptr;
    if (topItem) {   //Top item
        QStandardItemModel::appendRow(item);
        return rowCount() - 1;   //The return value is the index of top item.
    } else {   //Sub item
        int count = this->rowCount();
        for (int i = 0; i < count; i++) {
            const QModelIndex &index = this->index(i, 0);
            if (!index.isValid())
                continue;
            QString groupId = index.data(SideBarItem::Roles::kItemGroupRole).toString();
            if (groupId == DefaultGroup::kOther)
                groupOther = this->itemFromIndex(i);
            if (groupId != item->group())
                continue;
            SideBarItem *groupItem = this->itemFromIndex(i);
            bool itemInserted = false;
            for (int row = 0; row < groupItem->rowCount(); row++) {
                QStandardItem *childItem = groupItem->child(row);
                auto tmpItem = dynamic_cast<SideBarItem *>(childItem);
                if (!tmpItem)
                    continue;

                //Sort for devices group and network group, all so for quick access group.
                //Both of Computer plugin and bookmark plugin are following the the `hook_Group_Sort` event.
                bool sorted = { dpfHookSequence->run("dfmplugin_sidebar", "hook_Group_Sort", groupId, item->subGourp(), item->url(), tmpItem->url()) };
                if (sorted) {
                    groupItem->insertRow(row, item);
                    itemInserted = true;
                    break;
                }
            }
            if (!itemInserted)
                groupItem->appendRow(item);

            return groupItem->row() - 1;
        }
    }
    if (groupOther && !topItem) {   //If can not find out the parent item, just append it to Group_Other
        groupOther->appendRow(item);
        return groupOther->row() - 1;
    }
    QStandardItemModel::appendRow(item);
    return rowCount() - 1;
}

bool SideBarModel::removeRow(SideBarItem *item)
{
    // TODO(zhuangshu): To support tree view
    if (!item)
        return false;

    for (int row = rowCount() - 1; row >= 0; row--) {
        auto foundItem = dynamic_cast<SideBarItem *>(this->item(row, 0));
        if (item == foundItem) {
            QStandardItemModel::removeRow(row);
            return true;
        }
    }

    return false;
}

bool SideBarModel::removeRow(const QUrl &url)
{
    if (!url.isValid())
        return false;

    int count = this->rowCount();
    for (int i = 0; i < count; i++) {
        const QModelIndex &index = this->index(i, 0);   //top item index
        if (index.isValid()) {
            QStandardItem *item = qobject_cast<const SideBarModel *>(index.model())->itemFromIndex(index);
            SideBarItemSeparator *groupItem = dynamic_cast<SideBarItemSeparator *>(item);
            if (!groupItem)
                continue;
            int childCount = groupItem->rowCount();
            for (int j = 0; j < childCount; j++) {
                QStandardItem *childItem = groupItem->child(j);
                SideBarItem *subItem = dynamic_cast<SideBarItem *>(childItem);
                if (!subItem)
                    continue;
                if ((subItem->url().scheme() == url.scheme() && subItem->url().path() == url.path())) {
                    QStandardItemModel::removeRows(j, 1, groupItem->index());
                    return true;
                }
            }
        }
    }

    return false;
}

void SideBarModel::updateRow(const QUrl &url, const ItemInfo &newInfo)
{
    if (!url.isValid())
        return;
    for (int r = 0; r < rowCount(); r++) {
        auto item = itemFromIndex(r);   //Top item
        SideBarItemSeparator *groupItem = dynamic_cast<SideBarItemSeparator *>(item);
        if (!groupItem)
            continue;
        int childCount = groupItem->rowCount();
        for (int j = 0; j < childCount; j++) {
            QStandardItem *childItem = groupItem->child(j);
            SideBarItem *subItem = dynamic_cast<SideBarItem *>(childItem);
            if (!subItem)
                continue;
            bool foundByCb = subItem->itemInfo().findMeCb && subItem->itemInfo().findMeCb(subItem->url(), url);
            if (foundByCb || (subItem->url().scheme() == url.scheme() && subItem->url().path() == url.path())) {
                subItem->setIcon(newInfo.icon);
                subItem->setText(newInfo.displayName);
                subItem->setUrl(newInfo.url);
                subItem->setFlags(newInfo.flags);
                subItem->setGroup(newInfo.group);
                Qt::ItemFlags flags = subItem->flags();
                if (newInfo.isEditable)
                    flags |= Qt::ItemIsEditable;
                else
                    flags &= (~Qt::ItemIsEditable);
                subItem->setFlags(flags);
                return;
            }
        }
    }
}

/*
QStringList SideBarModel::groups() const
{
    QStringList list;
    auto controller = QtConcurrent::run([&]() {
        for (int row = rowCount() - 1; row <= 0; row--) {
            auto findedItem = dynamic_cast<SideBarItem *>(this->item(row, 0));
            if (!list.contains(findedItem->group()))
                list.push_front(findedItem->group());
        }
    });
    controller.waitForFinished();
    return list;
}
*/

QModelIndex SideBarModel::findRowByUrl(const QUrl &url) const
{
    QModelIndex retIndex;

    int count = this->rowCount();   //The top row count
    for (int i = 0; i < count; i++) {
        const QModelIndex &index = this->index(i, 0);   //top item index
        if (index.isValid()) {
            QStandardItem *item = qobject_cast<const SideBarModel *>(index.model())->itemFromIndex(index);
            SideBarItemSeparator *groupItem = dynamic_cast<SideBarItemSeparator *>(item);
            if (!groupItem)
                continue;
            int childCount = groupItem->rowCount();
            for (int j = 0; j < childCount; j++) {
                QStandardItem *childItem = groupItem->child(j);
                SideBarItem *subItem = dynamic_cast<SideBarItem *>(childItem);
                if (!subItem)
                    continue;
                if (DFMBASE_NAMESPACE::UniversalUtils::urlEquals(url, subItem->url())) {
                    return subItem->index();
                }
            }
        }
    }

    return retIndex;
}