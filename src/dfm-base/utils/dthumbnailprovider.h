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

#ifndef DFM_DTHUMBNAILPROVIDER_H
#define DFM_DTHUMBNAILPROVIDER_H

#include "dfm_base_global.h"

#include <QThread>
#include <QFileInfo>

#include <functional>

class QMimeType;

DFMBASE_BEGIN_NAMESPACE

class DThumbnailProviderPrivate;

class DThumbnailProvider : public QThread
{
    Q_OBJECT

public:
    enum Size : uint16_t {
        kSmall = 64,
        kNormal = 128,
        kLarge = 256,
    };

    static DThumbnailProvider *instance();

    bool hasThumbnail(const QFileInfo &info) const;
    bool hasThumbnail(const QMimeType &mimeType) const;

    QString thumbnailFilePath(const QFileInfo &info, Size size) const;

    typedef std::function<void(const QString &)> CallBack;
    QString createThumbnail(const QFileInfo &info, Size size);
    void appendToProduceQueue(const QFileInfo &info, Size size, CallBack callback = 0);
    void removeInProduceQueue(const QFileInfo &info, Size size);

    QString errorString() const;

    qint64 defaultSizeLimit() const;
    void setDefaultSizeLimit(qint64 size);

    qint64 sizeLimit(const QMimeType &mimeType) const;
    void setSizeLimit(const QMimeType &mimeType, qint64 size);

signals:
    void thumbnailChanged(const QString &sourceFilePath, const QString &thumbnailPath) const;
    void createThumbnailFinished(const QString &sourceFilePath, const QString &thumbnailPath) const;
    void createThumbnailFailed(const QString &sourceFilePath) const;

protected:
    explicit DThumbnailProvider(QObject *parent = 0);
    ~DThumbnailProvider() override;

    void run() override;

private:
    QScopedPointer<DThumbnailProviderPrivate> d;
};

DFMBASE_END_NAMESPACE

#endif   // DFM_DTHUMBNAILPROVIDER_H