/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huanyu<huanyub@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
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
#include "private/windowsservice_p.h"

#include "dfm-base/base/urlroute.h"
#include "dfm-base/base/application/application.h"
#include "dfm-base/base/application/settings.h"
#include "dfm-base/utils/finallyutil.h"
#include "dfm-base/shortcut/shortcut.h"

#include <QDebug>
#include <QEvent>
#include <QApplication>
#include <QX11Info>
#include <QScreen>
#include <QWindow>

DFMBASE_USE_NAMESPACE
DSB_FM_BEGIN_NAMESPACE

using DFMBASE_NAMESPACE::Application;
using DFMBASE_NAMESPACE::FileManagerWindow;
using DFMBASE_NAMESPACE::UrlRoute;

enum NetWmState {
    kNetWmStateAbove = 0x1,
    kNetWmStateBelow = 0x2,
    kNetWmStateFullScreen = 0x4,
    kNetWmStateMaximizedHorz = 0x8,
    kNetWmStateMaximizedVert = 0x10,
    kNetWmStateModal = 0x20,
    kNetWmStateStaysOnTop = 0x40,
    kNetWmStateDemandsAttention = 0x80
};
Q_DECLARE_FLAGS(NetWmStates, NetWmState)

/*!
 * \class WindowServicePrivate
 * \brief private class for `WindowsService`
 */

WindowsServicePrivate::WindowsServicePrivate(WindowsService *serv)
    : QObject(nullptr), service(serv)
{
}

FileManagerWindow *WindowsServicePrivate::activeExistsWindowByUrl(const QUrl &url)
{
    int count = windows.count();

    for (int i = 0; i != count; ++i) {
        quint64 key = windows.keys().at(i);
        auto window = windows.value(key);
        if (window && window->currentUrl() == url) {
            qInfo() << "Find url: " << url << " window: " << window;
            if (window->isMinimized())
                window->setWindowState(window->windowState() & ~Qt::WindowMinimized);
            qApp->setActiveWindow(window);
            return window;
        }
    }

    return nullptr;
}

void WindowsServicePrivate::moveWindowToScreenCenter(FileManagerWindow *window)
{
    QPoint pos = QCursor::pos();
    QRect currentScreenGeometry;

    for (QScreen *screen : qApp->screens()) {
        if (screen->geometry().contains(pos)) {
            currentScreenGeometry = screen->geometry();
        }
    }

    if (currentScreenGeometry.isEmpty()) {
        currentScreenGeometry = qApp->primaryScreen()->geometry();
    }

    window->moveCenter(currentScreenGeometry.center());
}

bool WindowsServicePrivate::isValidUrl(const QUrl &url, QString *error)
{
    Q_ASSERT(error);

    if (url.isEmpty()) {
        *error = "Can't new window use empty url";
        return false;
    }

    if (!url.isValid()) {
        *error = "Can't new window use not valid ur";
        return false;
    }

    if (!UrlRoute::hasScheme(url.scheme())) {
        *error = QString("No related scheme is registered "
                         "in the route form %0")
                         .arg(url.scheme());
        return false;
    }
    return true;
}

void WindowsServicePrivate::loadWindowState(DFMBASE_NAMESPACE::FileManagerWindow *window)
{
    const QVariantMap &state = Application::appObtuselySetting()->value("WindowManager", "WindowState").toMap();

    int width = state.value("width").toInt();
    int height = state.value("height").toInt();
    NetWmStates windowState = static_cast<NetWmStates>(state.value("state").toInt());

    // fix bug 30932,获取全屏属性，必须是width全屏和height全屏熟悉都满足，才判断是全屏
    if ((windows.size() == 0) && ((windowState & kNetWmStateMaximizedHorz) != 0 && (windowState & kNetWmStateMaximizedVert) != 0)) {
        window->showMaximized();
    } else {
        window->resize(width, height);
    }
}

void WindowsServicePrivate::saveWindowState(DFMBASE_NAMESPACE::FileManagerWindow *window)
{
    /// The power by dxcb platform plugin
    NetWmStates states = static_cast<NetWmStates>(window->window()->windowHandle()->property("_d_netWmStates").toInt());
    QVariantMap state;
    // fix bug 30932,获取全屏属性，必须是width全屏和height全屏熟悉都满足，才判断是全屏
    if ((states & kNetWmStateMaximizedHorz) == 0 || (states & kNetWmStateMaximizedVert) == 0) {
        state["width"] = window->size().width();
        state["height"] = window->size().height();
    } else {
        const QVariantMap &state1 = Application::appObtuselySetting()->value("WindowManager", "WindowState").toMap();
        state["width"] = state1.value("width").toInt();
        state["height"] = state1.value("height").toInt();
        state["state"] = static_cast<int>(states);
    }
    Application::appObtuselySetting()->setValue("WindowManager", "WindowState", state);
}

void WindowsServicePrivate::onWindowClosed(DFMBASE_NAMESPACE::FileManagerWindow *window)
{
    int count = windows.count();
    if (count <= 0)
        return;

    if (!window)
        return;

    // TODO(zhangs): window can destruct ?
    if (count == 1) {   // last window
        window->deleteLater();
        saveWindowState(window);
        // TODO(zhangs): close all property dialog
    } else {
        // fix bug 59239 drag事件的接受者的drop事件和发起drag事件的发起者的mousemove事件处理完成才能
        // 析构本窗口，检查当前窗口是否可以析构
        QPointer<FileManagerWindow> pwindow = window;
        QTimer::singleShot(1000, this, [=]() {
            pwindow->deleteLater();
        });
        qInfo() << "window deletelater !";
    }

    windows.remove(window->internalWinId());
}

void WindowsServicePrivate::onShowHotkeyHelp(FileManagerWindow *window)
{
    QRect rect = window->geometry();
    QPoint pos(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
    Shortcut sc;
    QStringList args;
    QString param1 = "-j=" + sc.toStr();
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());
    args << param1 << param2;
    QProcess::startDetached("deepin-shortcut-viewer", args);
}

/*!
 * \class WindowService
 * \brief manage all windows for filemanager
 */

WindowsService *WindowsService::service()
{
    auto &ctx = dpfInstance.serviceContext();
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [&ctx]() {
        if (!ctx.load(name()))
            abort();
    });

    return ctx.service<WindowsService>(name());
}

WindowsService::WindowsService(QObject *parent)
    : dpf::PluginService(parent),
      dpf::AutoServiceRegister<WindowsService>(),
      d(new WindowsServicePrivate(this))
{
}

WindowsService::~WindowsService()
{
    for (auto val : d->windows.values()) {
        // 框架退出非程序退出，依然会存在QWidget相关操作，
        // 如果强制使用delete，那么将导致Qt机制的与懒汉单例冲突崩溃
        if (val)
            val->deleteLater();
    }
    d->windows.clear();
}

void WindowsService::setCustomWindowCreator(WindowsService::WindowCreator creator)
{
    d->customCreator = creator;
}

/*!
 * \brief WindowsService::showWindow
 * \param url
 * \param isNewWindow
 * \param errorString
 * \return
 */
WindowsService::FMWindow *WindowsService::showWindow(const QUrl &url, bool isNewWindow, QString *errorString)
{
    Q_ASSERT_X(thread() == qApp->thread(), "WindowsService", "Show window must in main thread!");
    QString error;
    DFMBASE_NAMESPACE::FinallyUtil finally([&]() { if (errorString) *errorString = error; });

    QUrl showedUrl { url.isEmpty() ? Application::instance()->appUrlAttribute(Application::kUrlOfNewWindow) : url };
    if (!d->isValidUrl(showedUrl, &error)) {
        qWarning() << "Url: " << showedUrl << "is Invalid, error: " << error;
        // use home as showed url if default url is invalid
        showedUrl = UrlRoute::pathToReal(QDir::home().path());
        if (!d->isValidUrl(showedUrl, &error))
            return nullptr;
    }

    // Directly active window if the window exists
    if (!isNewWindow) {
        auto window = d->activeExistsWindowByUrl(showedUrl);
        if (window)
            return window;
        else
            qWarning() << "Cannot find a exists window by url: " << showedUrl;
    }

    QX11Info::setAppTime(QX11Info::appUserTime());

    // you can inherit from FMWindow to implement a custom window (by call `setCustomWindowCreator`)
    FMWindow *window = d->customCreator ? d->customCreator(showedUrl)
                                        : new FMWindow(showedUrl);

    window->show();
    d->loadWindowState(window);
    connect(window, &FileManagerWindow::aboutToClose, this, [this, window]() {
        emit windowClosed(window->internalWinId());
        d->onWindowClosed(window);
    });

    connect(window, &FileManagerWindow::reqShowHotkeyHelp, this, [this, window]() {
        d->onShowHotkeyHelp(window);
    });

    // In order for the plugin to cache the current window (before the base frame is installed)
    emit windowCreated(window->internalWinId());

    qInfo() << "New window created: " << window->winId() << showedUrl;
    d->windows.insert(window->internalWinId(), window);
    // TODO(zhangs): requestToSelectUrls

    if (d->windows.size() == 1)
        d->moveWindowToScreenCenter(window);

    finally.dismiss();
    qApp->setActiveWindow(window);
    emit windowOpened(window->internalWinId());
    return window;
}

/*!
 * \brief WindowsService::findWindowId find window id by input a window object pointer
 * \param window
 * \return window id
 */
quint64 WindowsService::findWindowId(const QWidget *window)
{
    if (!window)
        return 0;

    int count = d->windows.size();
    for (int i = 0; i != count; ++i) {
        quint64 key = d->windows.keys().at(i);
        if (d->windows.value(key) == window->topLevelWidget())
            return key;
    }

    // if `window` not contains `d->windows`, find the windows's base class
    const QWidget *newWindow = window;
    while (newWindow) {
        if (newWindow->inherits("FileManagerWindow"))
            return newWindow->winId();

        newWindow = newWindow->parentWidget();
    }

    return window->window()->internalWinId();
}

/*!
 * \brief WindowsService::findWindowById find a window by a window id
 * \param winId
 * \return window object pointer otherwise return nullptr
 */
WindowsService::FMWindow *WindowsService::findWindowById(quint64 winId)
{
    if (winId <= 0)
        return nullptr;

    if (d->windows.contains(winId))
        return d->windows.value(winId);

    qWarning() << "The `d->windows` cannot find winId: " << winId;
    for (QWidget *top : qApp->topLevelWidgets()) {
        if (top->internalWinId() == winId)
            return qobject_cast<WindowsService::FMWindow *>(top);
    }

    return nullptr;
}

/*!
 * \brief WindowsService::windowIdList
 * \return all windows id
 */
QList<quint64> WindowsService::windowIdList()
{
    return d->windows.keys();
}

DSB_FM_END_NAMESPACE