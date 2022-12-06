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

#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include "dfm_base_global.h"

#include <QThread>
#include <QFileInfo>

#include <functional>

class QMimeType;

namespace dfmbase {

class ThumbnailProviderPrivate;

class ThumbnailProvider : public QThread
{
    Q_OBJECT

public:
    struct ThumbNailCreateFuture
    {
        std::atomic_bool finished { 0 };
        QString thumbPath;
    };

public:
    enum Size : uint16_t {
        kSmall = 64,
        kNormal = 128,
        kLarge = 256,
    };

    static ThumbnailProvider *instance();

    bool hasThumbnail(const QUrl &url) const;
    bool hasThumbnail(const QMimeType &mimeType) const;
    int hasThumbnailFast(const QString &mimeType) const;

    QString thumbnailFilePath(const QUrl &fileUrl, Size size) const;

    QString createThumbnail(const QUrl &url, Size size);

    using CallBack = std::function<void(const QString &)>;
    void appendToProduceQueue(const QUrl url, Size size, QSharedPointer<ThumbNailCreateFuture> future);

    QString errorString() const;
    qint64 sizeLimit(const QMimeType &mimeType) const;

Q_SIGNALS:
    void createThumbnailFinished(const QUrl &sourceFilePath, const QString &thumbnailPath) const;
    void createThumbnailFailed(const QString &sourceFilePath) const;

private:
    void createAudioThumbnail(const QString &filePath, ThumbnailProvider::Size size, QScopedPointer<QImage> &image);
    bool createImageVDjvuThumbnail(const QString &filePath, ThumbnailProvider::Size size, QScopedPointer<QImage> &image, const QString &thumbnailName, QString &thumbnail);
    void createImageThumbnail(const QUrl &url, const QMimeType &mime, const QString &filePath, ThumbnailProvider::Size size, QScopedPointer<QImage> &image);
    void createTextThumbnail(const QString &filePath, ThumbnailProvider::Size size, QScopedPointer<QImage> &image);
    void createPdfThumbnail(const QString &filePath, ThumbnailProvider::Size size, QScopedPointer<QImage> &image);
    bool createDefaultThumbnail(const QMimeType &mime, const QString &filePath, ThumbnailProvider::Size size, QScopedPointer<QImage> &image, QString &thumbnail);
    bool createThumnailByMovieLib(const QString &filePath, QScopedPointer<QImage> &image);
    void initThumnailTool();
    bool createThumnailByDtkTools(const QMimeType &mime, ThumbnailProvider::Size size, const QString &filePath, QScopedPointer<QImage> &image);
    bool createThumnailByTools(const QMimeType &mime, ThumbnailProvider::Size size, const QString &filePath, QScopedPointer<QImage> &image);

protected:
    explicit ThumbnailProvider(QObject *parent = nullptr);
    ~ThumbnailProvider() override;

    void run() override;

private:
    QScopedPointer<ThumbnailProviderPrivate> d;
};

}

#endif   // THUMBNAILPROVIDER_H