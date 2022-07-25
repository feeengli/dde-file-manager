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
#include "customdatahandler.h"

#include <QDebug>

DDP_ORGANIZER_USE_NAMESPACE

CustomDataHandler::CustomDataHandler(QObject *parent)
    : CollectionDataProvider(parent)
    , ModelDataHandler()
{

}

CustomDataHandler::~CustomDataHandler()
{

}

void CustomDataHandler::check(const QSet<QUrl> &vaild)
{
    for (auto iter = collections.begin(); iter != collections.end(); ++iter) {
        for (auto it = iter.value()->items.begin(); it != iter.value()->items.end();) {
            if (vaild.contains(*it)) {
                ++it;
            } else {
                it = iter.value()->items.erase(it);
            }
        }
    }
}

QList<CollectionBaseDataPtr> CustomDataHandler::baseDatas() const
{
    return collections.values();
}

bool CustomDataHandler::addBaseData(const CollectionBaseDataPtr &base)
{
    if (!base || collections.contains(base->key))
        return false;

    collections.insert(base->key, base);
    return true;
}

void CustomDataHandler::removeBaseData(const QString &key)
{
    collections.remove(key);
}

bool CustomDataHandler::reset(const QList<CollectionBaseDataPtr> &datas)
{
    for (const CollectionBaseDataPtr &ptr : datas)
        collections.insert(ptr->key, ptr);

    return true;
}

QString CustomDataHandler::remove(const QUrl &url)
{
    for (auto iter = collections.begin(); iter != collections.end(); ++iter) {
        if (iter.value()->items.removeOne(url)) {
            emit itemsChanged(iter.key());
            return iter.key();
        }
    }

    return "";
}

QString CustomDataHandler::change(const QUrl &)
{
    return "";
}

QString CustomDataHandler::replace(const QUrl &oldUrl, const QUrl &newUrl)
{
    CollectionBaseDataPtr old;
    int oldIdx = -1;
    int newIdx = -1;
    for (auto iter = collections.begin(); iter != collections.end(); ++iter) {
        if (oldIdx < 0) {
            oldIdx = iter.value()->items.indexOf(oldUrl);
            if (oldIdx >= 0) {
                // find old
                old = iter.value();
            }
        }

        if (newIdx < 0)
            newIdx = iter.value()->items.indexOf(newUrl);

        if (oldIdx >= 0 && newIdx >= 0)
            break;
    }

    if (oldIdx < 0) {
        qWarning() << "replace: no old url:" << oldUrl;
        return "";
    }

    if (newIdx >= 0) {
        qWarning() << "replace: new url is existed:" << newUrl;
        return "";
    }

    Q_ASSERT(old);
    old->items.replace(oldIdx, newUrl);
    emit itemsChanged(old->key);

    return old->key;
}

QString CustomDataHandler::append(const QUrl &)
{
    return "";
}

void CustomDataHandler::insert(const QUrl &url, const QString &key, const int index)
{
    auto it = collections.find(key);
    if (Q_UNLIKELY(it == collections.end())) {
        CollectionBaseDataPtr base(new CollectionBaseData);
        base->key = key;
        base->items << url;
    } else {
        it.value()->items.insert(index, url);
    }

    emit itemsChanged(key);
}

bool CustomDataHandler::acceptInsert(const QUrl &url)
{
    // todo(wcl) 新建流程


    for (auto iter = collections.begin(); iter != collections.end(); ++iter) {
        if (iter.value()->items.contains(url)) {
           return true;
        }
    }

    return false;
}

QList<QUrl> CustomDataHandler::acceptReset(const QList<QUrl> &urls)
{
    QList<QUrl> ret;
    for (const QUrl &url : urls) {
        for (auto iter = collections.begin(); iter != collections.end(); ++iter) {
            if (iter.value()->items.contains(url)) {
                ret << url;
            }
        }
    }

    return ret;
}

bool CustomDataHandler::acceptRename(const QUrl &oldUrl, const QUrl &newUrl)
{
    for (auto iter = collections.begin(); iter != collections.end(); ++iter) {
        if (iter.value()->items.contains(oldUrl)
                || iter.value()->items.contains(newUrl))
            return true;
    }

    return false;
}