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
#include "fileoperations.h"
#include "fileoperationsevent/fileoperationseventreceiver.h"
#include "dfm-base/base/urlroute.h"
#include "dfm-base/base/schemefactory.h"
#include "dfm-base/dfm_event_defines.h"
#include "dfm-base/dfm_global_defines.h"
#include "dfm_global_defines.h"

DFMBASE_USE_NAMESPACE
DFMGLOBAL_USE_NAMESPACE
DPFILEOPERATIONS_USE_NAMESPACE

void FileOperations::initialize()
{
    initEventHandle();
}

bool FileOperations::start()
{
    return true;
}

/*!
 * \brief FileOperations::initEventHandle Initialize all event handling
 */
void FileOperations::initEventHandle()
{
    dpfSignalDispatcher->subscribe(GlobalEventType::kCopy,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>, const QUrl,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback)>(&FileOperationsEventReceiver::handleOperationCopy));
    dpfSignalDispatcher->subscribe(GlobalEventType::kCutFile,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const QUrl, const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback)>(&FileOperationsEventReceiver::handleOperationCut));
    dpfSignalDispatcher->subscribe(GlobalEventType::kRestoreFromTrash,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback)>(&FileOperationsEventReceiver::handleOperationRestoreFromTrash));
    dpfSignalDispatcher->subscribe(GlobalEventType::kMoveToTrash,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback)>(&FileOperationsEventReceiver::handleOperationMoveToTrash));
    dpfSignalDispatcher->subscribe(GlobalEventType::kDeleteFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback)>(&FileOperationsEventReceiver::handleOperationDeletes));
    dpfSignalDispatcher->subscribe(GlobalEventType::kCleanTrash,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::DeleteDialogNoticeType,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback)>(&FileOperationsEventReceiver::handleOperationCleanTrash));
    dpfSignalDispatcher->subscribe(GlobalEventType::kCopy,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const QUrl, const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback, const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationCopy));
    dpfSignalDispatcher->subscribe(GlobalEventType::kCutFile,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const QUrl, const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback, const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationCut));
    dpfSignalDispatcher->subscribe(GlobalEventType::kRestoreFromTrash,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback, const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationRestoreFromTrash));
    dpfSignalDispatcher->subscribe(GlobalEventType::kMoveToTrash,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback, const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationMoveToTrash));
    dpfSignalDispatcher->subscribe(GlobalEventType::kDeleteFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback, const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationDeletes));
    dpfSignalDispatcher->subscribe(GlobalEventType::kCleanTrash,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QList<QUrl>,
                                                                                     DFMGLOBAL_NAMESPACE::OperatorHandleCallback, const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationCleanTrash));

    dpfSignalDispatcher->subscribe(GlobalEventType::kOpenFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationOpenFiles));
    dpfSignalDispatcher->subscribe(GlobalEventType::kOpenFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>)>(&FileOperationsEventReceiver::handleOperationOpenFiles));
    dpfSignalDispatcher->subscribe(GlobalEventType::kOpenFilesByApp,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QList<QString>)>(&FileOperationsEventReceiver::handleOperationOpenFilesByApp));
    dpfSignalDispatcher->subscribe(GlobalEventType::kOpenFilesByApp,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QList<QString>,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationOpenFilesByApp));
    dpfSignalDispatcher->subscribe(GlobalEventType::kRenameFile,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QUrl,
                                                                                     const QUrl,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags)>(&FileOperationsEventReceiver::handleOperationRenameFile));
    dpfSignalDispatcher->subscribe(GlobalEventType::kRenameFile,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QUrl,
                                                                                     const QUrl,
                                                                                     const DFMBASE_NAMESPACE::AbstractJobHandler::JobFlags,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationRenameFile));

    dpfSignalDispatcher->subscribe(GlobalEventType::kRenameFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QPair<QString, QString>,
                                                                                     const bool replace)>(&FileOperationsEventReceiver::handleOperationRenameFiles));
    dpfSignalDispatcher->subscribe(GlobalEventType::kRenameFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QPair<QString, QString>,
                                                                                     const bool replace,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationRenameFiles));

    dpfSignalDispatcher->subscribe(GlobalEventType::kRenameFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QPair<QString, AbstractJobHandler::FileNameAddFlag>)>(&FileOperationsEventReceiver::handleOperationRenameFiles));
    dpfSignalDispatcher->subscribe(GlobalEventType::kRenameFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QPair<QString, AbstractJobHandler::FileNameAddFlag>,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationRenameFiles));

    dpfSignalDispatcher->subscribe(GlobalEventType::kMkdir,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64, const QUrl)>(&FileOperationsEventReceiver::handleOperationMkdir));
    dpfSignalDispatcher->subscribe(GlobalEventType::kMkdir,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64, const QUrl,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationMkdir));
    dpfSignalDispatcher->subscribe(GlobalEventType::kTouchFile,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<QString (FileOperationsEventReceiver::*)(const quint64,
                                                                                        const QUrl,
                                                                                        const CreateFileType,
                                                                                        const QString)>(&FileOperationsEventReceiver::handleOperationTouchFile));
    dpfSignalDispatcher->subscribe(GlobalEventType::kTouchFile,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QUrl,
                                                                                     const CreateFileType,
                                                                                     const QString,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationTouchFile));
    dpfSignalDispatcher->subscribe(GlobalEventType::kCreateSymlink,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QUrl,
                                                                                     const QUrl,
                                                                                     const bool force,
                                                                                     const bool silence)>(&FileOperationsEventReceiver::handleOperationLinkFile));
    dpfSignalDispatcher->subscribe(GlobalEventType::kCreateSymlink,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QUrl,
                                                                                     const QUrl,
                                                                                     const bool force,
                                                                                     const bool silence,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationLinkFile));
    dpfSignalDispatcher->subscribe(GlobalEventType::kSetPermission,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QUrl,
                                                                                     const QFileDevice::Permissions)>(&FileOperationsEventReceiver::handleOperationSetPermission));
    dpfSignalDispatcher->subscribe(GlobalEventType::kSetPermission,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QUrl,
                                                                                     const QFileDevice::Permissions,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationSetPermission));
    dpfSignalDispatcher->subscribe(GlobalEventType::kWriteUrlsToClipboard,
                                   FileOperationsEventReceiver::instance(),
                                   &FileOperationsEventReceiver::handleOperationWriteToClipboard);
    dpfSignalDispatcher->subscribe(GlobalEventType::kOpenInTerminal,
                                   FileOperationsEventReceiver::instance(),
                                   &FileOperationsEventReceiver::handleOperationOpenInTerminal);
    dpfSignalDispatcher->subscribe(GlobalEventType::kWriteCustomToClipboard,
                                   FileOperationsEventReceiver::instance(),
                                   &FileOperationsEventReceiver::handleOperationWriteDataToClipboard);
    dpfSignalDispatcher->subscribe(GlobalEventType::kSaveOperator,
                                   FileOperationsEventReceiver::instance(),
                                   &FileOperationsEventReceiver::handleOperationSaveOperations);
    dpfSignalDispatcher->subscribe(GlobalEventType::kCleanSaveOperator,
                                   FileOperationsEventReceiver::instance(),
                                   &FileOperationsEventReceiver::handleOperationCleanSaveOperationsStack);
    dpfSignalDispatcher->subscribe(GlobalEventType::kRevocation,
                                   FileOperationsEventReceiver::instance(),
                                   &FileOperationsEventReceiver::handleOperationRevocation);
    dpfSignalDispatcher->subscribe(GlobalEventType::kHideFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<bool (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>)>(&FileOperationsEventReceiver::handleOperationHideFiles));
    dpfSignalDispatcher->subscribe(GlobalEventType::kHideFiles,
                                   FileOperationsEventReceiver::instance(),
                                   static_cast<void (FileOperationsEventReceiver::*)(const quint64,
                                                                                     const QList<QUrl>,
                                                                                     const QVariant,
                                                                                     OperatorCallback)>(&FileOperationsEventReceiver::handleOperationHideFiles));
}