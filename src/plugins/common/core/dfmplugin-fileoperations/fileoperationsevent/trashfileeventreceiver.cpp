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
#include "trashfileeventreceiver.h"
#include "fileoperations/filecopymovejob.h"
#include "fileoperationsevent/fileoperationseventhandler.h"

#include "dfm-base/utils/hidefilehelper.h"
#include "dfm-base/base/urlroute.h"
#include "dfm-base/file/local/localfilehandler.h"
#include "dfm-base/utils/windowutils.h"
#include "dfm-base/dfm_event_defines.h"
#include "dfm-base/base/standardpaths.h"
#include "dfm-base/interfaces/abstractfileinfo.h"
#include "dfm-base/base/schemefactory.h"
#include "dfm-base/utils/fileutils.h"
#include "dfm-base/base/application/application.h"
#include "dfm-base/utils/decorator/decoratorfile.h"
#include "dfm-base/utils/decorator/decoratorfileinfo.h"
#include "dfm-base/utils/decorator/decoratorfileenumerator.h"
#include "dfm-base/utils/properties.h"
#include "dfm-base/utils/systempathutil.h"

#include <dfm-io/dfmio_utils.h>

#include <QDebug>

Q_DECLARE_METATYPE(QList<QUrl> *)
Q_DECLARE_METATYPE(bool *)
Q_DECLARE_METATYPE(QString *)

DFMGLOBAL_USE_NAMESPACE
DFMBASE_USE_NAMESPACE
DPFILEOPERATIONS_USE_NAMESPACE

TrashFileEventReceiver::TrashFileEventReceiver(QObject *parent)
    : QObject(parent)
{
    copyMoveJob.reset(new FileCopyMoveJob);
}

JobHandlePointer TrashFileEventReceiver::doMoveToTrash(const quint64 windowId, const QList<QUrl> sources, const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags flags,
                                                       DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback)
{
    Q_UNUSED(windowId);

    if (sources.isEmpty())
        return nullptr;

    if (SystemPathUtil::instance()->checkContainsSystemPath(sources)) {
        DialogManagerInstance->showDeleteSystemPathWarnDialog(windowId);
        return nullptr;
    }

    if (dpfHookSequence->run("dfmplugin_fileoperations", "hook_Operation_MoveToTrash", windowId, sources, flags)) {
        return nullptr;
    }

    const QUrl &sourceFirst = sources.first();
    JobHandlePointer handle = nullptr;
    if (FileUtils::isGvfsFile(sourceFirst) || DFMIO::DFMUtils::fileIsRemovable(sourceFirst)) {
        if (DialogManagerInstance->showDeleteFilesDialog(sources) != QDialog::Accepted)
            return nullptr;
        handle = copyMoveJob->deletes(sources, flags);
    } else {
        // check url permission
        QList<QUrl> urlsCanTrash = sources;
        auto it = urlsCanTrash.begin();
        while (it != urlsCanTrash.end()) {
            auto info = InfoFactory::create<AbstractFileInfo>(*it);
            if (!info || !info->canAttributes(AbstractFileInfo::FileCanType::kCanTrash))
                it = urlsCanTrash.erase(it);
            else
                ++it;
        }

        if (urlsCanTrash.isEmpty())
            return nullptr;

        if (!flags.testFlag(AbstractJobHandler::JobFlag::kRevocation) && Application::instance()->genericAttribute(Application::kShowDeleteConfirmDialog).toBool()) {
            if (DialogManagerInstance->showNormalDeleteConfirmDialog(urlsCanTrash) != QDialog::Accepted)
                return nullptr;
        }
        handle = copyMoveJob->moveToTrash(urlsCanTrash, flags);
    }
    if (handleCallback)
        handleCallback(handle);
    return handle;
}

JobHandlePointer TrashFileEventReceiver::doRestoreFromTrash(const quint64 windowId, const QList<QUrl> sources, const QUrl target,
                                                            const AbstractJobHandler::JobFlags flags, OperatorHandleCallback handleCallback)
{
    Q_UNUSED(windowId)

    if (sources.isEmpty())
        return nullptr;

    JobHandlePointer handle = copyMoveJob->restoreFromTrash(sources, target, flags);
    if (handleCallback)
        handleCallback(handle);
    return handle;
}

JobHandlePointer TrashFileEventReceiver::doCopyFromTrash(const quint64 windowId, const QList<QUrl> sources, const QUrl target, const AbstractJobHandler::JobFlags flags, OperatorHandleCallback handleCallback)
{
    Q_UNUSED(windowId)

    if (sources.isEmpty())
        return nullptr;

    JobHandlePointer handle = copyMoveJob->copyFromTrash(sources, target, flags);
    if (handleCallback)
        handleCallback(handle);
    return handle;
}

JobHandlePointer TrashFileEventReceiver::doCleanTrash(const quint64 windowId, const QList<QUrl> sources, const AbstractJobHandler::DeleteDialogNoticeType deleteNoticeType, OperatorHandleCallback handleCallback)
{
    Q_UNUSED(windowId)
    Q_UNUSED(deleteNoticeType)

    if (!sources.isEmpty()) {
        // Show delete files dialog
        if (DialogManagerInstance->showDeleteFilesDialog(sources) != QDialog::Accepted)
            return nullptr;
    } else {
        // Show clear trash dialog
        DecoratorFileEnumerator enumerator(QUrl("trash:"));
        if (DialogManagerInstance->showClearTrashDialog(enumerator.fileCount()) != QDialog::Accepted) return nullptr;
    }

    QList<QUrl> urls = std::move(sources);
    if (urls.isEmpty())
        urls.push_back(FileUtils::trashRootUrl());

    JobHandlePointer handle = copyMoveJob->cleanTrash(urls);
    if (handleCallback)
        handleCallback(handle);
    return handle;
}

TrashFileEventReceiver *TrashFileEventReceiver::instance()
{
    static TrashFileEventReceiver receiver;
    return &receiver;
}

void TrashFileEventReceiver::handleOperationMoveToTrash(const quint64 windowId, const QList<QUrl> sources, const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags flags,
                                                        DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback)
{
    auto handle = doMoveToTrash(windowId, sources, flags, handleCallback);
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kMoveToTrashType, handle);
}

void TrashFileEventReceiver::handleOperationRestoreFromTrash(const quint64 windowId, const QList<QUrl> sources, const QUrl target,
                                                             const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags flags,
                                                             DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback)
{
    auto handle = doRestoreFromTrash(windowId, sources, target, flags, handleCallback);
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kRestoreType, handle);
}

void TrashFileEventReceiver::handleOperationCleanTrash(const quint64 windowId, const QList<QUrl> sources, const AbstractJobHandler::DeleteDialogNoticeType deleteNoticeType,
                                                       DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback)
{
    auto handle = doCleanTrash(windowId, sources, deleteNoticeType, handleCallback);
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kCleanTrashType, handle);
}

void TrashFileEventReceiver::handleOperationMoveToTrash(const quint64 windowId,
                                                        const QList<QUrl> sources,
                                                        const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags flags,
                                                        DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback,
                                                        const QVariant custom,
                                                        DFMBASE_NAMESPACE::Global::OperatorCallback callback)
{

    JobHandlePointer handle = doMoveToTrash(windowId, sources, flags, handleCallback);
    if (callback) {
        CallbackArgus args(new QMap<CallbackKey, QVariant>);
        args->insert(CallbackKey::kWindowId, QVariant::fromValue(windowId));
        args->insert(CallbackKey::kJobHandle, QVariant::fromValue(handle));
        args->insert(CallbackKey::kCustom, custom);
        callback(args);
    }
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kMoveToTrashType, handle);
}

void TrashFileEventReceiver::handleOperationRestoreFromTrash(const quint64 windowId,
                                                             const QList<QUrl> sources, const QUrl target,
                                                             const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags flags,
                                                             DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback,
                                                             const QVariant custom,
                                                             DFMBASE_NAMESPACE::Global::OperatorCallback callback)
{
    JobHandlePointer handle = doRestoreFromTrash(windowId, sources, target, flags, handleCallback);
    if (callback) {
        CallbackArgus args(new QMap<CallbackKey, QVariant>);
        args->insert(CallbackKey::kWindowId, QVariant::fromValue(windowId));
        args->insert(CallbackKey::kJobHandle, QVariant::fromValue(handle));
        args->insert(CallbackKey::kCustom, custom);
        callback(args);
    }
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kRestoreType, handle);
}

void TrashFileEventReceiver::handleOperationCleanTrash(const quint64 windowId, const QList<QUrl> sources,
                                                       DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback,
                                                       const QVariant custom, OperatorCallback callback)
{
    JobHandlePointer handle = doCleanTrash(windowId, sources, AbstractJobHandler::DeleteDialogNoticeType::kEmptyTrash, handleCallback);
    if (callback) {
        CallbackArgus args(new QMap<CallbackKey, QVariant>);
        args->insert(CallbackKey::kWindowId, QVariant::fromValue(windowId));
        args->insert(CallbackKey::kJobHandle, QVariant::fromValue(handle));
        args->insert(CallbackKey::kCustom, custom);
        callback(args);
    }
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kCleanTrashType, handle);
}

void TrashFileEventReceiver::handleOperationCopyFromTrash(const quint64 windowId, const QList<QUrl> sources, const QUrl target,
                                                          const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags flags,
                                                          DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback)
{
    auto handle = doCopyFromTrash(windowId, sources, target, flags, handleCallback);
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kRestoreType, handle);
}

void TrashFileEventReceiver::handleOperationCopyFromTrash(const quint64 windowId,
                                                          const QList<QUrl> sources, const QUrl target,
                                                          const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags flags,
                                                          DFMGLOBAL_NAMESPACE::OperatorHandleCallback handleCallback,
                                                          const QVariant custom,
                                                          DFMBASE_NAMESPACE::Global::OperatorCallback callback)
{
    JobHandlePointer handle = doCopyFromTrash(windowId, sources, target, flags, handleCallback);
    if (callback) {
        CallbackArgus args(new QMap<CallbackKey, QVariant>);
        args->insert(CallbackKey::kWindowId, QVariant::fromValue(windowId));
        args->insert(CallbackKey::kJobHandle, QVariant::fromValue(handle));
        args->insert(CallbackKey::kCustom, custom);
        callback(args);
    }
    FileOperationsEventHandler::instance()->handleJobResult(AbstractJobHandler::JobType::kRestoreType, handle);
}