/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuzhangjian<liqianga@uniontech.com>
 *
 * Maintainer: liuzhangjian<liqianga@uniontech.com>
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
#ifndef WORKSPACEMENUSCENE_H
#define WORKSPACEMENUSCENE_H

#include "dfmplugin_workspace_global.h"

#include <interfaces/abstractmenuscene.h>
#include <interfaces/abstractscenecreator.h>

DPWORKSPACE_BEGIN_NAMESPACE

class WorkspaceMenuCreator : public DFMBASE_NAMESPACE::AbstractSceneCreator
{
public:
    static QString name()
    {
        return "WorkspaceMenu";
    }
    DFMBASE_NAMESPACE::AbstractMenuScene *create() override;
};

class WorkspaceMenuScenePrivate;
class WorkspaceMenuScene : public DFMBASE_NAMESPACE::AbstractMenuScene
{
public:
    explicit WorkspaceMenuScene(QObject *parent = nullptr);
    QString name() const override;
    bool initialize(const QVariantHash &params) override;
    AbstractMenuScene *scene(QAction *action) const override;
    bool create(QMenu *parent) override;
    void updateState(QMenu *parent) override;
    bool triggered(QAction *action) override;

private:
    void createEmptyMenu(QMenu *parent);
    void createNormalMenu(QMenu *parent);
    bool emptyMenuTriggered(QAction *action);
    bool normalMenuTriggered(QAction *action);

    QMenu *displayAsSubActions(QMenu *menu);
    QMenu *sortBySubActions(QMenu *menu);
    void sortByRole(int role);
    void updateEmptyAreaActionState();

private:
    WorkspaceMenuScenePrivate *const d = nullptr;
};

DPWORKSPACE_END_NAMESPACE
#endif   // WORKSPACEMENUSCENE_H