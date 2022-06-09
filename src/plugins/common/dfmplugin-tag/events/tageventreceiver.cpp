/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuyangming<liuyangming@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             max-lv<lvwujun@uniontech.com>
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
#include "tageventreceiver.h"
#include "utils/tagmanager.h"

#include "dfm-base/dfm_event_defines.h"

#include <dfm-framework/dpf.h>

DFMBASE_USE_NAMESPACE
DPTAG_USE_NAMESPACE

TagEventReceiver *TagEventReceiver::instance()
{
    static TagEventReceiver ins;
    return &ins;
}

void TagEventReceiver::initConnect()
{
    dpfSignalDispatcher->subscribe(GlobalEventType::kCutFileResult, this, &TagEventReceiver::handleFileCutResult);
    dpfSignalDispatcher->subscribe(GlobalEventType::kMoveToTrashResult, this, &TagEventReceiver::handleFileRemoveResult);
    dpfSignalDispatcher->subscribe(GlobalEventType::kDeleteFilesResult, this, &TagEventReceiver::handleFileRemoveResult);
}

void TagEventReceiver::handleFileCutResult(const QList<QUrl> &srcUrls, const QList<QUrl> &destUrls, bool ok, const QString &errMsg)
{
    Q_UNUSED(errMsg)

    if (!ok)
        return;

    for (const QUrl &url : srcUrls) {
        QStringList tags = TagManager::instance()->getTagsByUrls({ url });
        if (!tags.isEmpty()) {
            TagManager::instance()->removeTagsOfFiles(tags, { url });

            const QUrl &newUrl = destUrls.at(srcUrls.indexOf(url));
            TagManager::instance()->addTagsForFiles(tags, { newUrl });
        }
    }
}

void TagEventReceiver::handleFileRemoveResult(const QList<QUrl> &srcUrls, bool ok, const QString &errMsg)
{
    Q_UNUSED(errMsg)

    if (!ok)
        return;

    for (const QUrl &url : srcUrls) {
        QStringList tags = TagManager::instance()->getTagsByUrls({ url });
        if (!tags.isEmpty()) {
            TagManager::instance()->removeTagsOfFiles(tags, { url });
        }
    }
}

TagEventReceiver::TagEventReceiver(QObject *parent)
    : QObject(parent)
{
}