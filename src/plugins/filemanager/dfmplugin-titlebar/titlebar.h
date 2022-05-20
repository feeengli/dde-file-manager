/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangsheng<zhangsheng@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
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
#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "dfmplugin_titlebar_global.h"

#include <dfm-framework/dpf.h>

DPTITLEBAR_BEGIN_NAMESPACE

class TitleBar : public dpf::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.plugin.filemanager" FILE "titlebar.json")

    DPF_EVENT_NAMESPACE(DPTITLEBAR_NAMESPACE)
    // singnal events
    DPF_EVENT_REG_SIGNAL(signal_StartSearch)
    DPF_EVENT_REG_SIGNAL(signal_StopSearch)
    DPF_EVENT_REG_SIGNAL(signal_ShowFilterView)

    // slot events
    DPF_EVENT_REG_SLOT(slot_StartSpinner)
    DPF_EVENT_REG_SLOT(slot_StopSpinner)
    DPF_EVENT_REG_SLOT(slot_ShowFilterButton)

public:
    virtual void initialize() override;
    virtual bool start() override;
    virtual ShutdownFlag stop() override;

private slots:
    void onWindowCreated(quint64 windId);
    void onWindowOpened(quint64 windId);
    void onWindowClosed(quint64 windId);
    void bindEvents();
};

DPTITLEBAR_END_NAMESPACE

#endif   // TITLEBAR_H
