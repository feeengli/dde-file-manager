/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liyigang<liyigang@uniontech.com>
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
#ifndef FILEOPERATIONSPLUGIN_H
#define FILEOPERATIONSPLUGIN_H

#include "dfmplugin_fileoperations_global.h"

#include <dfm-framework/dpf.h>

DPFILEOPERATIONS_BEGIN_NAMESPACE
class FileOperations : public dpf::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.plugin.common" FILE "fileoperations.json")

    DPF_EVENT_NAMESPACE(DPFILEOPERATIONS_NAMESPACE)

    // hook events
    DPF_EVENT_REG_HOOK(hook_OpenLocalFiles)

    // hook events -- file operation
    DPF_EVENT_REG_HOOK(hook_Operation_CopyFile)
    DPF_EVENT_REG_HOOK(hook_Operation_CutFile)
    DPF_EVENT_REG_HOOK(hook_Operation_DeleteFile)
    DPF_EVENT_REG_HOOK(hook_Operation_MoveToTrash)
    DPF_EVENT_REG_HOOK(hook_Operation_RestoreFromTrash)
    DPF_EVENT_REG_HOOK(hook_Operation_CleanTrash)
    DPF_EVENT_REG_HOOK(hook_Operation_OpenFileInPlugin)
    DPF_EVENT_REG_HOOK(hook_Operation_OpenFileByApp)
    DPF_EVENT_REG_HOOK(hook_Operation_OpenInTerminal)
    DPF_EVENT_REG_HOOK(hook_Operation_RenameFile)
    DPF_EVENT_REG_HOOK(hook_Operation_MakeDir)
    DPF_EVENT_REG_HOOK(hook_Operation_TouchFile)
    DPF_EVENT_REG_HOOK(hook_Operation_LinkFile)
    DPF_EVENT_REG_HOOK(hook_Operation_SetPermission)
    DPF_EVENT_REG_HOOK(hook_Operation_WriteUrlsToClipboard)
    DPF_EVENT_REG_HOOK(hook_Operation_RenameFiles)
    DPF_EVENT_REG_HOOK(hook_Operation_RenameFilesAddText)

public:
    virtual void initialize() override;
    virtual bool start() override;
    virtual ShutdownFlag stop() override;

private slots:
    void initEventHandle();
};

DPFILEOPERATIONS_END_NAMESPACE
#endif   // COREPLUGIN_H