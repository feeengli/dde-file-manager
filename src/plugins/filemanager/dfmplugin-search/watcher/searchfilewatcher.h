/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuzhangjian<liuzhangjian@uniontech.com>
 *
 * Maintainer: liuzhangjian<liuzhangjian@uniontech.com>
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
#ifndef SEARCHFILEWATCHER_H
#define SEARCHFILEWATCHER_H

#include "dfmplugin_search_global.h"

#include "dfm-base/interfaces/abstractfilewatcher.h"

DPSEARCH_BEGIN_NAMESPACE

class SearchFileWatcherPrivate;
class SearchFileWatcher : public DFMBASE_NAMESPACE::AbstractFileWatcher
{
    Q_OBJECT
public:
    explicit SearchFileWatcher() = delete;
    explicit SearchFileWatcher(const QUrl &url, QObject *parent = nullptr);
    ~SearchFileWatcher() override;
    virtual void setEnabledSubfileWatcher(const QUrl &subfileUrl, bool enabled = true) override;

private:
    void addWatcher(const QUrl &url);
    void removeWatcher(const QUrl &url);

    void onFileDeleted(const QUrl &url);
    void onFileAttributeChanged(const QUrl &url);
    void onFileRenamed(const QUrl &fromUrl, const QUrl &toUrl);

    SearchFileWatcherPrivate *dptr;
};

DPSEARCH_END_NAMESPACE

#endif   // SEARCHFILEWATCHER_H