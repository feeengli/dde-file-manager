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
#include "searcheventcaller.h"
#include "utils/searchhelper.h"

#include "services/filemanager/workspace/workspace_defines.h"
#include "services/filemanager/titlebar/titlebar_defines.h"

#include "dfm-base/dfm_event_defines.h"

#include <dfm-framework/dpf.h>

DPSEARCH_BEGIN_NAMESPACE

void dfmplugin_search::SearchEventCaller::sendDoSearch(quint64 winId, const QUrl &url)
{
    DFMBASE_USE_NAMESPACE
    dpfSignalDispatcher->publish(GlobalEventType::kChangeCurrentUrl, winId, url);
}

void SearchEventCaller::sendShowAdvanceSearchBar(quint64 winId, bool visible)
{
    DSB_FM_USE_NAMESPACE
    dpfSignalDispatcher->publish(Workspace::EventType::kShowCustomTopWidget, winId, SearchHelper::scheme(), visible);
}

void SearchEventCaller::sendShowAdvanceSearchButton(quint64 winId, bool visible)
{
    DSB_FM_USE_NAMESPACE
    dpfSlotChannel->push("dfmplugin_titlebar", "slot_ShowFilterButton", winId, visible);
}

void SearchEventCaller::sendStartSpinner(quint64 winId)
{
    DSB_FM_USE_NAMESPACE
    dpfSlotChannel->push("dfmplugin_titlebar", "slot_StartSpinner", winId);
}

void SearchEventCaller::sendStopSpinner(quint64 winId)
{
    DSB_FM_USE_NAMESPACE
    dpfSlotChannel->push("dfmplugin_titlebar", "slot_StopSpinner", winId);
}

DPSEARCH_END_NAMESPACE
