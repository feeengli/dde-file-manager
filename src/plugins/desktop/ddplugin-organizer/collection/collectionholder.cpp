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
#include "collectionholder_p.h"
#include "view/collectionframe.h"
#include "view/collectionwidget.h"
#include "models/fileproxymodel.h"
#include "desktoputils/ddpugin_eventinterface_helper.h"

DDP_ORGANIZER_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

CollectionHolderPrivate::CollectionHolderPrivate(const QString &uuid, CollectionDataProvider *dataProvider, CollectionHolder *qq, QObject *parent)
    : QObject(qq)
    , q(qq)
    , id(uuid)
    , provider(dataProvider)
{

}

CollectionHolderPrivate::~CollectionHolderPrivate()
{
    if (frame) {
        frame->setParent(nullptr);
        delete frame;
    }

    if (widget) {
        widget->setParent(nullptr);
        delete widget;
    }
}

void CollectionHolderPrivate::onAdjustFrameSize(const CollectionFrameSize &size)
{
    // todo:从网格或视图获取图标rect
    QRect iconRect(0, 0, 92, 115);
    QRect collectionRect = frame->geometry();
    if (size == CollectionFrameSize::kSmall) {
        collectionRect.setHeight(iconRect.height() * 2);
        frame->setGeometry(collectionRect);
    } else if (size == CollectionFrameSize::kLarge) {
        collectionRect.setHeight(iconRect.height() * 4);
        frame->setGeometry(collectionRect);
    }
}

CollectionHolder::CollectionHolder(const QString &uuid, ddplugin_organizer::CollectionDataProvider *dataProvider, QObject *parent)
    : QObject(parent)
    , d(new CollectionHolderPrivate(uuid, dataProvider, this))
{
    d->styleTimer.setSingleShot(true);
    d->styleTimer.setInterval(500);

    connect(&d->styleTimer, &QTimer::timeout, this, [this](){
        emit styleChanged(id());
    });
}

CollectionHolder::~CollectionHolder()
{

}

void CollectionHolder::setCanvasModelShell(CanvasModelShell *sh)
{
    d->widget->setCanvasModelShell(sh);
}

void CollectionHolder::setCanvasViewShell(CanvasViewShell *sh)
{
    d->widget->setCanvasViewShell(sh);
}

void CollectionHolder::setCanvasGridShell(CanvasGridShell *sh)
{
    d->widget->setCanvasGridShell(sh);
}

QString CollectionHolder::id() const
{
    return d->id;
}

QString CollectionHolder::name()
{
    return d->widget->titleName();
}

void CollectionHolder::setName(const QString &text)
{
    d->widget->setTitleName(text);
}

DFrame *CollectionHolder::frame() const
{
    return d->frame;
}

void CollectionHolder::createFrame(Surface *surface, FileProxyModel *model)
{
    d->surface = surface;

    d->frame = new CollectionFrame(surface);

    d->model = model;
    d->widget = new CollectionWidget(d->id, d->provider, d->frame);
    d->widget->setModel(d->model);
    d->widget->setGeometry(QRect(QPoint(0, 0), d->frame->size()));

    d->frame->setWidget(d->widget);

    connect(d->widget, &CollectionWidget::sigRequestClose, this, &CollectionHolder::sigRequestClose);
    connect(d->widget, &CollectionWidget::sigRequestAdjustSize, d.data(), &CollectionHolderPrivate::onAdjustFrameSize);
    connect(d->frame, &CollectionFrame::geometryChanged, this, [this](){
        d->styleTimer.start();
    });
}

Surface *CollectionHolder::surface() const
{
    return d->surface;
}

void CollectionHolder::show()
{
    d->frame->show();
}

void CollectionHolder::setMovable(const bool movable)
{
    auto features = d->frame->collectionFeatures();
    if (movable)
        features |= CollectionFrame::CollectionFrameMovable;
    else
        features &= ~CollectionFrame::CollectionFrameMovable;

    d->frame->setCollectionFeatures(features);
}

bool CollectionHolder::movable() const
{
    return d->frame->collectionFeatures().testFlag(CollectionFrame::CollectionFrameMovable);
}

void CollectionHolder::setClosable(const bool closable)
{
    auto features = d->frame->collectionFeatures();
    if (closable)
        features |= CollectionFrame::CollectionFrameClosable;
    else
        features &= ~CollectionFrame::CollectionFrameClosable;

    d->frame->setCollectionFeatures(features);
    d->widget->setClosable(closable);
}

bool CollectionHolder::closable() const
{
    return d->frame->collectionFeatures().testFlag(CollectionFrame::CollectionFrameClosable);
}

void CollectionHolder::setFloatable(const bool floatable)
{
    auto features = d->frame->collectionFeatures();
    if (floatable)
        features |= CollectionFrame::CollectionFrameFloatable;
    else
        features &= ~CollectionFrame::CollectionFrameFloatable;

    d->frame->setCollectionFeatures(features);
}

bool CollectionHolder::floatable() const
{
    return d->frame->collectionFeatures().testFlag(CollectionFrame::CollectionFrameFloatable);
}

void CollectionHolder::setHiddableCollection(const bool hiddable)
{
    auto features = d->frame->collectionFeatures();
    if (hiddable)
        features |= CollectionFrame::CollectionFrameHiddable;
    else
        features &= ~CollectionFrame::CollectionFrameHiddable;

    d->frame->setCollectionFeatures(features);
}

bool CollectionHolder::hiddableCollection() const
{
    return d->frame->collectionFeatures().testFlag(CollectionFrame::CollectionFrameHiddable);
}

void CollectionHolder::setStretchable(const bool stretchable)
{
    auto features = d->frame->collectionFeatures();
    if (stretchable)
        features |= CollectionFrame::CollectionFrameStretchable;
    else
        features &= ~CollectionFrame::CollectionFrameStretchable;

    d->frame->setCollectionFeatures(features);
}

bool CollectionHolder::stretchable() const
{
    return d->frame->collectionFeatures().testFlag(CollectionFrame::CollectionFrameStretchable);
}

void CollectionHolder::setAdjustable(const bool adjustable)
{
    auto features = d->frame->collectionFeatures();
    if (adjustable)
        features |= CollectionFrame::CollectionFrameAdjustable;
    else
        features &= ~CollectionFrame::CollectionFrameAdjustable;

    d->frame->setCollectionFeatures(features);
    d->widget->setAdjustable(adjustable);
}

bool CollectionHolder::adjustable() const
{
    return d->frame->collectionFeatures().testFlag(CollectionFrame::CollectionFrameAdjustable);
}

void CollectionHolder::setHiddableTitleBar(const bool hiddable)
{
    // todo
}

bool CollectionHolder::hiddableTitleBar() const
{
    // todo
}

void CollectionHolder::setHiddableView(const bool hiddable)
{
    // todo
}

bool CollectionHolder::hiddableView() const
{
    // todo
}

void CollectionHolder::setRenamable(const bool renamable)
{
    d->widget->setRenamable(renamable);
}

bool CollectionHolder::renamable() const
{
    return d->widget->renamable();
}

void CollectionHolder::setDragEnabled(bool enable)
{
    d->widget->setDragEnabled(enable);
}

bool CollectionHolder::dragEnabled() const
{
    return d->widget->dragEnabled();
}

void CollectionHolder::setStyle(const CollectionStyle &style)
{
    if (style.rect.isValid())
        d->frame->setGeometry(style.rect);
}

CollectionStyle CollectionHolder::style() const
{
    CollectionStyle style;
    style.key = id();
    // todo get index.
    style.screenIndex = 1;

    style.rect = d->frame->geometry();
    return style;
}