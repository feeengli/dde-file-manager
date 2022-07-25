/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     wangchunlin<wangchunlin@uniontech.com>
 *
 * Maintainer: wangchunlin<wangchunlin@uniontech.com>
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
#include "fileoperator_p.h"

#include "dfm-base/dfm_event_defines.h"
#include "dfm-base/dfm_global_defines.h"

#include <QUrl>
#include <QVariant>

using namespace ddplugin_organizer;
using namespace dfmbase;
using namespace Global;

static constexpr char const kCollectionKey[] = "CollectionKey";
static constexpr char const kDropFilesIndex[] = "DropFilesIndex";


class FileOperatorGlobal : public FileOperator {};
Q_GLOBAL_STATIC(FileOperatorGlobal, fileOperatorGlobal)

FileOperatorPrivate::FileOperatorPrivate(FileOperator *qq)
    : q(qq)
{

}

void FileOperatorPrivate::callBackTouchFile(const QUrl &target, const QVariantMap &customData)
{
    // todo(wangcl) 右键菜单新建的回调响应流程
}

void FileOperatorPrivate::callBackPasteFiles(const JobInfoPointer info)
{
    // todo(wangcl) 文件粘贴、拖拽释放的回调响应流程。流程无法满足需求，待商榷方案。
    if (info->keys().contains(AbstractJobHandler::NotifyInfoKey::kCompleteTargetFilesKey)) {
        QList<QUrl> files = info->value(AbstractJobHandler::NotifyInfoKey::kCompleteTargetFilesKey).value<QList<QUrl>>();

        // todo(wangcl)
        // 如 dropFilesToCollection 的备注，此时文件是否全部创建，状态未知。
        // 即使通过延迟，让文件全部创建，依然存在文件先显示在桌面，再显示到集合的问题。
        // 另外，如果桌面本身存在同名文件，用户从文管拖拽文件到集合时选择了替换，此时文件显示到集合中？还是保持在桌面的原有位置？
    }
}

void FileOperatorPrivate::callBackRenameFiles(const QList<QUrl> &sources, const QList<QUrl> &targets)
{
    // todo(wangcl) 批量文件重命名的回调响应流程
}

FileOperator::FileOperator(QObject *parent)
    : QObject(parent)
    , d(new FileOperatorPrivate(this))
{
    d->callBack = std::bind(&FileOperator::callBackFunction, this, std::placeholders::_1);
}

FileOperator::~FileOperator()
{

}

FileOperator *FileOperator::instance()
{
    return fileOperatorGlobal;
}

void FileOperator::setDataProvider(CollectionDataProvider *provider)
{
    d->provider = provider;
}

void FileOperator::dropFilesToCollection(const Qt::DropAction &action, const QUrl &targetUrl, const QList<QUrl> &urls, const QString &key, const int index)
{
    // todo(wangcl) 逻辑流程无法满足需求，现有方案会导致文件先出现在桌面，再移动动到集合中
    /*!
      * 从文管drop文件到集合，只能是追加（效果与从文管拖拽到桌面一致，而原因，也与桌面一样)
      * 1.底层执行粘贴（拖拽释放也是粘贴）是异步执行，只有所有粘贴执行完成后，才会调用回调函数和发送事件，
      * 在此之前，watcher已经监测到文件的创建，并通知model创建了文件index，创建时由于没有回调函数提供的位置信息和新文件名信息，
      * 文件只能按顺序追加。
      * 2.另一方面，底层无法做到在执行粘贴之前，就返回所有的最终文件名，因为其是在执行的过程中，逐个获取最终文件名称的，
      * 因为存在同名文件时，需要弹窗让用户选择“共存”、“替换”或“跳过”选项。
      * 3.另一种情况，底层已经掉了回调和发送事件，而model还没有将所有文件创建完毕，此时在回调中直接调用选中文件，存在部分文件选中失败的可能。
      * 4.为了解决上述问题，可能的解决方案是在收到回调函数之后，延迟一定的时间，再去选中（原文管流程）。
      * 5.但是，对于文件存放集合的问题则无法通过延迟解决，且，在弹窗让用户选中共存、跳过等时，实际已经有部分文件被创建成功，
      * 由于缺少回调函数提供的位置信息，导致新创建的文件不会被集合劫持，从而会先显示在桌面上（按空位自动存放）。最后等回调的延迟超时后，
      * 才会从桌面移动到集合中！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
      * 6.只能底层修改逻辑，让执行粘贴之前，返回所有最终文件名？？？包括重名的文件，也需要在开始粘贴之前，让用户完成所有选择后，再开始执行粘贴操作。
      * 7.另外，即使在粘贴之前回调告诉了所有文件名称与所属集合信息，但是在执行粘贴的过程中，由于其他原因（比如用户手动修改了某个文件的名称），
      * 导致又出现了文件重名，此时还是需要弹窗让用户选择？那么又需要通过一个事件向外通知该特殊情况？
    */

    QVariantMap data;
    data.insert(kCollectionKey, key);
    data.insert(kDropFilesIndex, index);
    QPair<FileOperatorPrivate::CallBackFunc, QVariant> funcData(FileOperatorPrivate::kCallBackPasteFiles, data);
    QVariant custom = QVariant::fromValue(funcData);

    // default is copy file
    auto type = action == Qt::MoveAction ? GlobalEventType::kCutFile : GlobalEventType::kCopy;

    dpfSignalDispatcher->publish(type, 0, urls, targetUrl, AbstractJobHandler::JobFlag::kNoHint, nullptr, custom, d->callBack);
}

void FileOperator::dropFilesToCanvas(const Qt::DropAction &action, const QUrl &targetUrl, const QList<QUrl> &urls)
{
    // default is copy file
    auto type = action == Qt::MoveAction ? GlobalEventType::kCutFile : GlobalEventType::kCopy;
    dpfSignalDispatcher->publish(type, 0, urls, targetUrl, AbstractJobHandler::JobFlag::kNoHint, nullptr);
}

void FileOperator::dropToTrash(const QList<QUrl> &urls)
{
    dpfSignalDispatcher->publish(GlobalEventType::kMoveToTrash, 0, urls, AbstractJobHandler::JobFlag::kNoHint, nullptr);
}

void FileOperator::dropToApp(const QList<QUrl> &urls, const QString &app)
{
    QList<QString> apps { app };
    dpfSignalDispatcher->publish(GlobalEventType::kOpenFilesByApp, 0, urls, apps);
}

void FileOperator::callBackFunction(const Global::CallbackArgus args)
{
    const QVariant &customValue = args->value(CallbackKey::kCustom);
    const QPair<FileOperatorPrivate::CallBackFunc, QVariant> &custom = customValue.value<QPair<FileOperatorPrivate::CallBackFunc, QVariant>>();
    const FileOperatorPrivate::CallBackFunc funcKey = custom.first;

    switch (funcKey) {
    case FileOperatorPrivate::CallBackFunc::kCallBackTouchFile:
    case FileOperatorPrivate::CallBackFunc::kCallBackTouchFolder: {
        // Folder also belong to files
        // touch file is sync operation

        auto targets = args->value(CallbackKey::kTargets).value<QList<QUrl>>();
        if (Q_UNLIKELY(targets.count() != 1)) {
            qWarning() << "unknow error.touch file successed,target urls is:" << targets;
        }

        d->callBackTouchFile(targets.first(), custom.second.toMap());
    } break;
    case FileOperatorPrivate::CallBackFunc::kCallBackPasteFiles: {
        // paste files is async operation
        JobHandlePointer jobHandle = args->value(CallbackKey::kJobHandle).value<JobHandlePointer>();

        if (jobHandle->currentState() != AbstractJobHandler::JobState::kStopState) {
            connect(jobHandle.get(), &AbstractJobHandler::finishedNotify, d.get(), &FileOperatorPrivate::callBackPasteFiles);
        } else {
            JobInfoPointer infoPointer = jobHandle->getTaskInfoByNotifyType(AbstractJobHandler::NotifyType::kNotifyFinishedKey);
            d->callBackPasteFiles(infoPointer);
        }
    } break;
    case FileOperatorPrivate::CallBackFunc::kCallBackRenameFiles: {
        auto sources = args->value(CallbackKey::kSourceUrls).value<QList<QUrl>>();
        auto targets = args->value(CallbackKey::kTargets).value<QList<QUrl>>();
        d->callBackRenameFiles(sources, targets);
    } break;
    default:
        break;
    }
}
