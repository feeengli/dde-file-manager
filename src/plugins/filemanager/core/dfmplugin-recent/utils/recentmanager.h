/*
 * Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yanghao<yanghao@uniontech.com>
 *
 * Maintainer: huangyu<huangyub@uniontech.com>
 *             liuyangming<liuyangming@uniontech.com>
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
#ifndef RECENTMANAGER_H
#define RECENTMANAGER_H

#include "dfmplugin_recent_global.h"
#include "files/recentfileinfo.h"
#include "files/recentiterateworker.h"

#include "dfm-base/utils/clipboard.h"
#include "dfm-base/interfaces/abstractjobhandler.h"
#include "dfm-base/interfaces/abstractfilewatcher.h"
#include "dfm-base/widgets/dfmwindow/filemanagerwindowsmanager.h"
#include "dfm-base/dfm_global_defines.h"

#include <QUrl>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QFile>
#include <QQueue>
#include <QThread>
#include <QTimer>

namespace dfmplugin_recent {

class RecentManager final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(RecentManager)
    using BasicExpand = QMultiMap<QString, QPair<QString, QString>>;
    using ExpandFieldMap = QMap<QString, BasicExpand>;

public:
    static RecentManager *instance();

    inline static QString scheme()
    {
        return "recent";
    }

    inline static QIcon icon()
    {
        return QIcon::fromTheme("document-open-recent-symbolic");
    }

    inline static QString xbelPath()
    {
        return QDir::homePath() + "/.local/share/recently-used.xbel";
    }

    static QUrl rootUrl();

    static void clearRecent();
    static void contenxtMenuHandle(quint64 windowId, const QUrl &url, const QPoint &globalPos);
    static ExpandFieldMap propetyExtensionFunc(const QUrl &url);
    static QUrl urlTransform(const QUrl &url);

    QMap<QUrl, AbstractFileInfoPointer> getRecentNodes() const;
    bool removeRecentFile(const QUrl &url);

    bool customColumnRole(const QUrl &rootUrl, QList<DFMGLOBAL_NAMESPACE::ItemRoles> *roleList);
    bool customRoleDisplayName(const QUrl &url, const DFMGLOBAL_NAMESPACE::ItemRoles role, QString *displayName);
    bool detailViewIcon(const QUrl &url, QString *iconName);
    bool sepateTitlebarCrumb(const QUrl &url, QList<QVariantMap> *mapGroup);
    bool urlsToLocal(const QList<QUrl> &origins, QList<QUrl> *urls);
    bool isTransparent(const QUrl &url, DFMGLOBAL_NAMESPACE::TransparentStatus *status);

signals:
    void asyncHandleFileChanged();

private:
    explicit RecentManager(QObject *parent = nullptr);
    ~RecentManager() override;

    void init();
    void removeRecent(const QList<QUrl> &urls);

private slots:
    void updateRecent();
    void onUpdateRecentFileInfo(const QUrl &url, qint64 readTime);
    void onDeleteExistRecentUrls(QList<QUrl> &urls);

private:
    QTimer updateRecentTimer;
    QThread workerThread;
    RecentIterateWorker *iteratorWorker { new RecentIterateWorker };   // free by QThread::finished
    AbstractFileWatcherPointer watcher;
    QMap<QUrl, AbstractFileInfoPointer> recentNodes;
};
}
#endif   // RECENTMANAGER_H