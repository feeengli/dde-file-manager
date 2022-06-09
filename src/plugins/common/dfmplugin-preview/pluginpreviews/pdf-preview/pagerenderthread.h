/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     lixiang<lixianga@uniontech.com>
 *
 * Maintainer: lixiang<lixianga@uniontech.com>
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

#ifndef PAGERENDERTHREAD_H
#define PAGERENDERTHREAD_H

#include "preview_plugin_global.h"

#include "model.h"

#include <QThread>
#include <QMutex>
#include <QStack>
#include <QImage>
#include <QPixmap>

PREVIEW_BEGIN_NAMESPACE
class DocSheet;
class BrowserPage;
class SheetRenderer;
class SideBarImageViewModel;

struct DocPageNormalImageTask
{   //正常取图
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;   //任务艾迪
    QRect rect = QRect();   //整个大小
};

struct DocPageSliceImageTask
{   //取切片
    DocSheet *sheet = nullptr;
    BrowserPage *page = nullptr;
    int pixmapId = 0;   //任务艾迪
    QRect whole = QRect();   //整个大小
    QRect slice = QRect();   //切片大小
};

struct DocPageThumbnailTask
{   //缩略图
    DocSheet *sheet = nullptr;
    SideBarImageViewModel *model = nullptr;
    int index = -1;
};

struct DocOpenTask
{   //打开文档
    DocSheet *sheet = nullptr;
    QString password;
    SheetRenderer *renderer = nullptr;
};

struct DocCloseTask
{
    Document *document = nullptr;
    QList<Page *> pages;
};

/**
 * @brief The PageRenderThread class
 * 执行加载图片和文字等耗时操作的线程,由于pdfium非常线程不安全，所有操作都在本线程中进行
 */
class PageRenderThread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief clearImageTasks
     * 清除需要读取图片的任务
     * @param sheet
     * @param item 项指针
     * @param pixmapId 删除不同的pixmapId,-1为删除所有
     * @return 是否成功
     */
    static bool clearImageTasks(DocSheet *sheet, BrowserPage *page, int pixmapId = -1);

    /**
     * @brief appendTask
     * 添加任务到队列
     * @param task
     */
    static void appendTask(DocPageNormalImageTask task);

    static void appendTask(DocPageSliceImageTask task);

    static void appendTask(DocPageThumbnailTask task);

    static void appendTask(DocOpenTask task);

    static void appendTask(DocCloseTask task);

    /**
     * @brief destroyForever
     * 销毁线程且不会再被创建
     */
    static void destroyForever();

private:
    explicit PageRenderThread(QObject *parent = nullptr);

    ~PageRenderThread();

    void run();

private:
    bool hasNextTask();

    bool popNextDocPageNormalImageTask(DocPageNormalImageTask &task);

    bool popNextDocPageThumbnailTask(DocPageThumbnailTask &task);

    bool popNextDocOpenTask(DocOpenTask &task);

    bool popNextDocCloseTask(DocCloseTask &task);

private:
    bool execNextDocPageNormalImageTask();

    bool execNextDocPageThumbnailTask();

    bool execNextDocOpenTask();

    bool execNextDocCloseTask();

signals:
    void sigDocPageNormalImageTaskFinished(DocPageNormalImageTask, QPixmap);

    void sigDocPageThumbnailTaskFinished(DocPageThumbnailTask, QPixmap);

    void sigDocOpenTask(DocOpenTask, Document::Error, Document *, QList<Page *>);

private slots:
    void onDocPageNormalImageTaskFinished(DocPageNormalImageTask task, QPixmap pixmap);

    void onDocPageThumbnailTask(DocPageThumbnailTask task, QPixmap pixmap);

    void onDocOpenTask(DocOpenTask task, Document::Error error, Document *document, QList<Page *> pages);

private:
    static PageRenderThread *instance();

private:
    QMutex pageNormalImageMutex;
    QList<DocPageNormalImageTask> pageNormalImageTasks;

    QMutex pageSliceImageMutex;
    QList<DocPageSliceImageTask> pageSliceImageTasks;

    QMutex pageThumbnailMutex;
    QList<DocPageThumbnailTask> pageThumbnailTasks;

    QMutex openMutex;
    QList<DocOpenTask> openTasks;

    QMutex closeMutex;
    QList<DocCloseTask> closeTasks;

    bool quitDoc { false };

    static bool quitForever;

    static PageRenderThread *pageRenderThread;
};
PREVIEW_END_NAMESPACE
#endif   // PAGERENDERTHREAD_H