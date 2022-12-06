/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yanghao<yanghao@uniontech.com>
 *
 * Maintainer: liuyangming<liuyangming@uniontech.com>
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
#ifndef TRASHFILEINFO_H
#define TRASHFILEINFO_H

#include "dfmplugin_trashcore_global.h"
#include "dfm-base/interfaces/abstractfileinfo.h"

namespace dfmplugin_trashcore {

class TrashFileInfoPrivate;
class TrashFileInfo : public DFMBASE_NAMESPACE::AbstractFileInfo
{
    Q_GADGET
    friend class TrashFileInfoPrivate;

public:
    explicit TrashFileInfo(const QUrl &url);
    ~TrashFileInfo() override;

    virtual void refresh() override;
    virtual QString nameInfo(const FileNameInfoType type = FileNameInfoType::kFileName) const override;
    virtual QString displayInfo(const DisplayInfoType type = DisplayInfoType::kFileDisplayName) const override;
    virtual QString pathInfo(const FilePathInfoType type = FilePathInfoType::kFilePath) const override;
    virtual QUrl urlInfo(const FileUrlInfoType type = FileUrlInfoType::kUrl) const override;
    virtual bool exists() const override;
    virtual bool canAttributes(const FileCanType type = FileCanType::kCanDrag) const override;

    virtual bool isAttributes(const FileIsType type = FileIsType::kIsFile) const override;
    virtual QFile::Permissions permissions() const override;
    virtual QIcon fileIcon() override;

    virtual qint64 size() const override;
    virtual int countChildFile() const override;

    virtual QVariant timeInfo(const FileTimeType type = FileTimeType::kCreateTime) const override;
    virtual QVariant customData(int role) const override;

private:
    TrashFileInfoPrivate *d;
};

}

#endif   // TRASHFILEINFO_H