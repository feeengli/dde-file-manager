/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     xushitong<xushitong@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             zhangsheng<zhangsheng@uniontech.com>
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
#ifndef AVFSMENUSCENE_H
#define AVFSMENUSCENE_H

#include "dfmplugin_avfsbrowser_global.h"

#include "dfm-base/interfaces/abstractmenuscene.h"
#include "dfm-base/interfaces/abstractscenecreator.h"

DPAVFSBROWSER_BEGIN_NAMESPACE

class AvfsMenuSceneCreator : public DFMBASE_NAMESPACE::AbstractSceneCreator
{
public:
    inline static QString name()
    {
        return "AvfsMenu";
    }

    // AbstractSceneCreator interface
public:
    virtual DFMBASE_NAMESPACE::AbstractMenuScene *create() override;
};

class AvfsMenuScenePrivate;
class AvfsMenuScene : public DFMBASE_NAMESPACE::AbstractMenuScene
{
public:
    explicit AvfsMenuScene(QObject *parent = nullptr);
    virtual ~AvfsMenuScene() override;

    virtual QString name() const override;
    virtual bool initialize(const QVariantHash &params) override;
    virtual bool create(QMenu *parent) override;
    virtual void updateState(QMenu *parent) override;
    virtual bool triggered(QAction *action) override;
    virtual DFMBASE_NAMESPACE::AbstractMenuScene *scene(QAction *action) const override;

private:
    QScopedPointer<AvfsMenuScenePrivate> d;
};

DPAVFSBROWSER_END_NAMESPACE

#endif   // AVFSMENUSCENE_H