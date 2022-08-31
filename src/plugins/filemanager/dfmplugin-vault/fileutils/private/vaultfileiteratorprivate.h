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
#ifndef VAULTFILEDIRITERATORPTIVATE_H
#define VAULTFILEDIRITERATORPTIVATE_H

#include "dfmplugin_vault_global.h"
#include "dfm-base/interfaces/abstractdiriterator.h"
#include "dfm-base/interfaces/abstractfilewatcher.h"

#include <QDirIterator>
#include <QDebug>

#include <dfm-io/core/denumerator.h>
#include <dfm-io/dfmio_global.h>
#include <dfm-io/dfmio_register.h>
#include <dfm-io/core/diofactory.h>

namespace dfmplugin_vault {
class VaultFileIterator;
class VaultFileIteratorPrivate : public QObject
{
    Q_OBJECT
    friend class VaultFileIterator;

public:
    explicit VaultFileIteratorPrivate(const QUrl &url,
                                      const QStringList &nameFilters,
                                      QDir::Filters filters,
                                      QDirIterator::IteratorFlags flags,
                                      VaultFileIterator *qp);

    ~VaultFileIteratorPrivate();

private:
    QSharedPointer<dfmio::DEnumerator> dfmioDirIterator = nullptr;   // dfmio的文件迭代器
    QUrl currentUrl;   // 当前迭代器所在位置文件的url
    QDir::Filters curFilters;   // 文件的当前的过滤flags
    bool isCurrent = false;   // 用来判断当前是否使用了一次next
    VaultFileIterator *const q;
};

}

#endif   //VAULTFILEDIRITERATORPTIVATE_H