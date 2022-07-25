/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangyu<zhangyub@uniontech.com>
 *
 * Maintainer: zhangyu<zhangyub@uniontech.com>
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
#include "framemanager.h"
#include "private/framemanager_p.h"
#include "config/configpresenter.h"
#include "interface/canvasmodelshell.h"
#include "desktoputils/ddpugin_eventinterface_helper.h"
#include "menus/extendcanvasscene.h"

#include "plugins/common/dfmplugin-menu/menu_eventinterface_helper.h"
#include "dfm-base/dfm_desktop_defines.h"

#include <QAbstractItemView>

DFMBASE_USE_NAMESPACE
DDP_ORGANIZER_USE_NAMESPACE

#define CanvasCoreSubscribe(topic, func) \
    dpfSignalDispatcher->subscribe("ddplugin_core", QT_STRINGIFY2(topic), this, func);

#define CanvasCoreUnsubscribe(topic, func) \
    dpfSignalDispatcher->unsubscribe("ddplugin_core", QT_STRINGIFY2(topic), this, func);

FrameManagerPrivate::FrameManagerPrivate(FrameManager *qq)
    : QObject(qq)
    , q(qq)
{

}

FrameManagerPrivate::~FrameManagerPrivate()
{
    delete organizer;
    // 退出自动整理
    // 刷新桌面
}

void FrameManagerPrivate::buildSurface()
{
    QList<QWidget *> root = ddplugin_desktop_util::desktopFrameRootWindows();
    QMap<QString, QWidget *> rootMap;
    // create surface
    for (QWidget *win : root) {
        const QString screenName = win->property(DesktopFrameProperty::kPropScreenName).toString();
        if (screenName.isEmpty()) {
            qWarning() << "can not get screen name from root window";
            continue;
        }

        SurfacePointer surface = surfaceWidgets.value(screenName);
        if (surface.isNull()) {
            // add new widget
            qInfo() << "screen:" << screenName << "  added, create it.";
            surface = createSurface(win);
            surfaceWidgets.insert(screenName, surface);
        }

        layoutSurface(win, surface);
        rootMap.insert(screenName, win);
    }

    // clean up invalid widget
    {
        for (const QString &sp : surfaceWidgets.keys()) {
            if (!rootMap.contains(sp)) {
                surfaceWidgets.take(sp);
                qInfo() << "remove surface:" << sp;
            }
        }
    }
}

void FrameManagerPrivate::clearSurface()
{
    for (const SurfacePointer &sur : surfaceWidgets.values())
        sur->setParent(nullptr);

    surfaceWidgets.clear();
}

SurfacePointer FrameManagerPrivate::createSurface(QWidget *root)
{
    SurfacePointer surface = nullptr;
    if (Q_UNLIKELY(!root))
        return surface;

    surface.reset(new Surface());
    surface->setProperty(DesktopFrameProperty::kPropScreenName, root->property(DesktopFrameProperty::kPropScreenName).toString());
    surface->setProperty(DesktopFrameProperty::kPropWidgetName, "organizersurface");
    surface->setProperty(DesktopFrameProperty::kPropWidgetLevel, 11.0);
    return surface;
}

void FrameManagerPrivate::layoutSurface(QWidget *root, SurfacePointer surface, bool hidden)
{
    Q_ASSERT(surface);
    Q_ASSERT(root);

    auto view = dynamic_cast< QAbstractItemView *>(findView(root));
    // check hidden flags
    if (view && !hidden) {
        surface->setParent(view->viewport());
        surface->setGeometry(QRect(QPoint(0, 0), view->geometry().size()));
    } else {
        surface->setParent(root);
        surface->setGeometry(QRect(QPoint(0, 0), root->geometry().size()));
    }
}

void FrameManagerPrivate::buildOrganizer()
{
    q->switchMode(CfgPresenter->mode());
}

void FrameManagerPrivate::refeshCanvas()
{
    if (canvas)
        canvas->canvasModel()->refresh(1);
}

void FrameManagerPrivate::enableChanged(bool e)
{
    if (e == CfgPresenter->isEnable())
        return;

    qDebug() << "enableChanged" << e;
    CfgPresenter->setEnable(e);
    if (e)
        q->turnOn();
    else
        q->turnOff();
}

void FrameManagerPrivate::switchToCustom()
{
    Q_ASSERT(organizer);

    if (organizer->mode() == OrganizerMode::kCustom) {
        qDebug() << "reject to switch: current mode had been custom.";
        return;
    }

    CfgPresenter->setMode(kCustom);
    buildOrganizer();
}

void FrameManagerPrivate::switchToNormalized(int cf)
{
    Q_ASSERT(organizer);

    if (organizer->mode() == OrganizerMode::kNormalized) {
        CfgPresenter->setClassification(static_cast<Classifier>(cf));
        organizer->reset();
    } else {
        CfgPresenter->setMode(kNormalized);
        CfgPresenter->setClassification(static_cast<Classifier>(cf));
        buildOrganizer();
    }
}

QWidget *FrameManagerPrivate::findView(QWidget *root) const
{
    if (Q_UNLIKELY(!root))
        return nullptr;

    for (QObject *obj : root->children()) {
        if (QWidget *wid = dynamic_cast<QWidget *>(obj)) {
            QString type = wid->property(DesktopFrameProperty::kPropWidgetName).toString();
            if (type == "canvas") {
                return wid;
            }
        }
    }
    return nullptr;
}

FrameManager::FrameManager(QObject *parent)
    : QObject(parent)
    , d(new FrameManagerPrivate(this))
{

}

FrameManager::~FrameManager()
{
    turnOff();

    // unregister menu
    dfmplugin_menu_util::menuSceneUnbind(ExtendCanvasCreator::name());
    auto creator = dfmplugin_menu_util::menuSceneUnregisterScene(ExtendCanvasCreator::name());
    if (creator)
        delete creator;
}

bool FrameManager::initialize()
{
    // initialize config
    CfgPresenter->initialize();

    // register menu for canvas
    dfmplugin_menu_util::menuSceneRegisterScene(ExtendCanvasCreator::name(), new ExtendCanvasCreator());
    dfmplugin_menu_util::menuSceneBind(ExtendCanvasCreator::name(), "CanvasMenu");

    bool enable = CfgPresenter->isEnable();
    qInfo() << "Organizer enable:" << enable;
    if (enable)
        turnOn(false); // builded by signal.

    connect(CfgPresenter, &ConfigPresenter::changeEnableState, d, &FrameManagerPrivate::enableChanged);
    connect(CfgPresenter, &ConfigPresenter::switchToNormalized, d, &FrameManagerPrivate::switchToNormalized);
    connect(CfgPresenter, &ConfigPresenter::switchToCustom, d, &FrameManagerPrivate::switchToCustom);
    return true;
}

void FrameManager::layout()
{
    if (d->organizer)
        d->organizer->layout();
}

void FrameManager::switchMode(OrganizerMode mode)
{
    if (d->organizer)
        delete d->organizer;

    qInfo() << "switch to" << mode;

    d->organizer = OrganizerCreator::createOrganizer(mode);
    Q_ASSERT(d->organizer);

    connect(d->organizer, &CanvasOrganizer::collectionChanged, d, &FrameManagerPrivate::refeshCanvas);

    // initialize to create collection widgets
    if (!d->surfaceWidgets.isEmpty())
        d->organizer->setSurfaces(d->surfaceWidgets.values());

    d->organizer->setCanvasModelShell(d->canvas->canvasModel());
    d->organizer->setCanvasViewShell(d->canvas->canvasView());
    d->organizer->setCanvasGridShell(d->canvas->canvasGrid());
    d->organizer->initialize(d->model);
}

void FrameManager::turnOn(bool build)
{
    Q_ASSERT(!d->canvas);
    Q_ASSERT(!d->model);
    Q_ASSERT(!d->organizer);

    CanvasCoreSubscribe(signal_DesktopFrame_WindowAboutToBeBuilded, &FrameManager::onDetachWindows);
    CanvasCoreSubscribe(signal_DesktopFrame_WindowBuilded, &FrameManager::onBuild);
    CanvasCoreSubscribe(signal_DesktopFrame_WindowShowed, &FrameManager::onWindowShowed);
    CanvasCoreSubscribe(signal_DesktopFrame_GeometryChanged, &FrameManager::onGeometryChanged);
    CanvasCoreSubscribe(signal_DesktopFrame_AvailableGeometryChanged, &FrameManager::onGeometryChanged);

    d->canvas = new CanvasInterface(this);
    d->canvas->initialize();

    d->model = new FileProxyModel(this);
    d->model->setModelShell(d->canvas->fileInfoModel());

    if (build) {
        onBuild();

        // show surface
        for (const SurfacePointer &sur : d->surfaceWidgets.values())
            sur->setVisible(true);
    }
}

void FrameManager::turnOff()
{
    CanvasCoreUnsubscribe(signal_DesktopFrame_WindowAboutToBeBuilded, &FrameManager::onDetachWindows);
    CanvasCoreUnsubscribe(signal_DesktopFrame_WindowBuilded, &FrameManager::onBuild);
    CanvasCoreUnsubscribe(signal_DesktopFrame_WindowShowed, &FrameManager::onWindowShowed);
    CanvasCoreUnsubscribe(signal_DesktopFrame_GeometryChanged, &FrameManager::onGeometryChanged);
    CanvasCoreUnsubscribe(signal_DesktopFrame_AvailableGeometryChanged, &FrameManager::onGeometryChanged);

    delete d->organizer;
    d->organizer = nullptr;

    delete d->model;
    d->model = nullptr;

    d->clearSurface();

    // restore canvas
    d->refeshCanvas();

    delete d->canvas;
    d->canvas = nullptr;
}

void FrameManager::onBuild()
{
    d->buildSurface();

    if (d->organizer) {
        d->organizer->layout();
    } else {
        d->buildOrganizer();
    }
}

void FrameManager::onWindowShowed()
{
    // todo hide the canvas if needed.
}

void FrameManager::onDetachWindows()
{
    for (const SurfacePointer &sur : d->surfaceWidgets.values())
        sur->setParent(nullptr);

    // 解绑集合窗口
}

void FrameManager::onGeometryChanged()
{
    QList<QWidget *> root = ddplugin_desktop_util::desktopFrameRootWindows();
    for (auto win : root) {
        QString screen = win->property(DesktopFrameProperty::kPropScreenName).toString();
        auto surface = d->surfaceWidgets.value(screen);
        if (surface)
            d->layoutSurface(win, surface);
    }

    // layout collection widgets
    if (d->organizer)
        d->organizer->layout();
}
