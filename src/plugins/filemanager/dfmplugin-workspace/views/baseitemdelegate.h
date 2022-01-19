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
#ifndef BASEITEMDELEGATE_H
#define BASEITEMDELEGATE_H

#include "dfmplugin_workspace_global.h"

#include <QStyledItemDelegate>

DPWORKSPACE_BEGIN_NAMESPACE

class FileView;
class BaseItemDelegatePrivate;
class BaseItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BaseItemDelegate(FileView *parent);
    virtual ~BaseItemDelegate();

    /*!
     * \brief paintGeomertys paint geomertys for all items
     * \param option
     * \param index
     * \param sizeHintMode
     * \return
     */
    virtual QList<QRect> paintGeomertys(const QStyleOptionViewItem &option, const QModelIndex &index, bool sizeHintMode = false) const = 0;

    /**
     * @brief iconSizeLevel return icon size level
     * @return Return current icon level if level is vaild, otherwise return -1
     */
    virtual int iconSizeLevel() const;

    /**
     * @brief minimumIconSizeLevel return minimum icon size level
     * @return Return minimum icon level if level is vaild, otherwise return -1
     */
    virtual int minimumIconSizeLevel() const;

    /**
     * @brief maximumIconSizeLevel return maximum icon size level
     * @return Return maximum icon level if level is vaild, otherwise return -1
     */
    virtual int maximumIconSizeLevel() const;

    /**
     * @brief increaseIcon increase icon size level
     * @return  Return current icon level if icon can increase; otherwise return -1.
     */
    virtual int increaseIcon();

    /**
     * @brief increaseIcon decrease icon size level
     * @return  Return current icon level if icon can decrease; otherwise return -1.
     */
    virtual int decreaseIcon();

    /*!
     * \brief setIconSizeByIconSizeLevel set icon size level
     * \param level
     * \return  Return current icon level if level is vaild, otherwise return -1
     */
    virtual int setIconSizeByIconSizeLevel(int level);

    QScopedPointer<BaseItemDelegatePrivate> d;

protected:
    explicit BaseItemDelegate(BaseItemDelegatePrivate &dd, FileView *parent);

    Q_DECLARE_PRIVATE_D(d, BaseItemDelegate)
};

DPWORKSPACE_END_NAMESPACE

#endif   // BASEITEMDELEGATE_H