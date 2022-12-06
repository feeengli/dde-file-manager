/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lanxuesong<lanxuesong@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             zhangsheng<zhangsheng@uniontech.com>
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
#include "fileinfohelper.h"

DFMBASE_USE_NAMESPACE
FileInfoHelper::FileInfoHelper(QObject *parent)
    : QObject(parent)
{
    connect(ThumbnailProvider::instance(), &ThumbnailProvider::createThumbnailFinished,
            this, &FileInfoHelper::createThumbnailFinished);
    connect(ThumbnailProvider::instance(), &ThumbnailProvider::createThumbnailFailed,
            this, &FileInfoHelper::createThumbnailFailed);
}

FileInfoHelper::~FileInfoHelper()
{
}

FileInfoHelper &FileInfoHelper::instance()
{
    static FileInfoHelper helper;
    return helper;
}