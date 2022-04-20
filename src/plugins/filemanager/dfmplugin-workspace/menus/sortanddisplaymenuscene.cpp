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
#include "sortanddisplaymenuscene.h"
#include "sortanddisplaymenuscene_p.h"
#include "workspacemenu_defines.h"
#include "views/fileview.h"
#include "models/filesortfilterproxymodel.h"

#include "services/common/menu/menu_defines.h"
#include "dfm-base/dfm_global_defines.h"

#include <QMenu>

DFMBASE_USE_NAMESPACE
DPWORKSPACE_USE_NAMESPACE
DSC_USE_NAMESPACE

AbstractMenuScene *SortAndDisplayMenuCreator::create()
{
    return new SortAndDisplayMenuScene();
}

SortAndDisplayMenuScene::SortAndDisplayMenuScene(QObject *parent)
    : AbstractMenuScene(parent), d(new SortAndDisplayMenuScenePrivate(this))
{
    d->predicateName[ActionID::kSortBy] = tr("Sort by");
    d->predicateName[ActionID::kDisplayAs] = tr("Display as");

    // 排序子菜单
    d->predicateName[ActionID::kSrtName] = tr("Name");
    d->predicateName[ActionID::kSrtTimeModified] = tr("Time modified");
    d->predicateName[ActionID::kSrtSize] = tr("Size");
    d->predicateName[ActionID::kSrtType] = tr("Type");

    // 显示子菜单
    d->predicateName[ActionID::kDisplayIcon] = tr("Icon");
    d->predicateName[ActionID::kDisplayList] = tr("List");
}

SortAndDisplayMenuScene::~SortAndDisplayMenuScene()
{
}

QString SortAndDisplayMenuScene::name() const
{
    return SortAndDisplayMenuCreator::name();
}

bool SortAndDisplayMenuScene::initialize(const QVariantHash &params)
{
    d->isEmptyArea = params.value(MenuParamKey::kIsEmptyArea).toBool();
    return d->isEmptyArea;
}

AbstractMenuScene *SortAndDisplayMenuScene::scene(QAction *action) const
{
    if (action == nullptr)
        return nullptr;

    if (!d->predicateAction.key(action).isEmpty())
        return const_cast<SortAndDisplayMenuScene *>(this);

    return AbstractMenuScene::scene(action);
}

bool SortAndDisplayMenuScene::create(QMenu *parent)
{
    d->view = qobject_cast<FileView *>(parent->parent());
    d->createEmptyMenu(parent);
    return true;
}

void SortAndDisplayMenuScene::updateState(QMenu *parent)
{
    Q_UNUSED(parent);
    d->updateEmptyAreaActionState();
}

bool SortAndDisplayMenuScene::triggered(QAction *action)
{
    if (!d->view)
        return false;

    const auto &actionId = action->property(ActionPropertyKey::kActionID).toString();
    if (d->predicateAction.values().contains(action)) {
        // display as
        {
            // display as icon
            if (actionId == ActionID::kDisplayIcon) {
                d->view->setViewMode(Global::ViewMode::kIconMode);
                return true;
            }

            // display as list
            if (actionId == ActionID::kDisplayList) {
                d->view->setViewMode(Global::ViewMode::kListMode);
                return true;
            }
        }

        // sort by
        {
            // sort by name
            if (actionId == ActionID::kSrtName) {
                d->sortByRole(Global::ItemRoles::kItemNameRole);
                return true;
            }

            // sort by time modified
            if (actionId == ActionID::kSrtTimeModified) {
                d->sortByRole(Global::ItemRoles::kItemFileLastModifiedRole);
                return true;
            }

            // sort by size
            if (actionId == ActionID::kSrtSize) {
                d->sortByRole(Global::ItemRoles::kItemFileSizeRole);
                return true;
            }

            // sort by size
            if (actionId == ActionID::kSrtType) {
                d->sortByRole(Global::ItemRoles::kItemFileMimeTypeRole);
                return true;
            }
        }
    }

    return false;
}

SortAndDisplayMenuScenePrivate::SortAndDisplayMenuScenePrivate(AbstractMenuScene *qq)
    : AbstractMenuScenePrivate(qq)
{
}

void SortAndDisplayMenuScenePrivate::createEmptyMenu(QMenu *parent)
{
    QAction *tempAction = parent->addAction(predicateName.value(ActionID::kDisplayAs));
    tempAction->setMenu(addDisplayAsActions(parent));
    predicateAction[ActionID::kDisplayAs] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kDisplayAs));

    tempAction = parent->addAction(predicateName.value(ActionID::kSortBy));
    tempAction->setMenu(addSortByActions(parent));
    predicateAction[ActionID::kSortBy] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kSortBy));
}

QMenu *SortAndDisplayMenuScenePrivate::addSortByActions(QMenu *menu)
{
    QMenu *subMenu = new QMenu(menu);

    // SortBy
    QAction *tempAction = subMenu->addAction(predicateName.value(ActionID::kSrtName));
    tempAction->setCheckable(true);
    predicateAction[ActionID::kSrtName] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kSrtName));

    tempAction = subMenu->addAction(predicateName.value(ActionID::kSrtTimeModified));
    tempAction->setCheckable(true);
    predicateAction[ActionID::kSrtTimeModified] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kSrtTimeModified));

    tempAction = subMenu->addAction(predicateName.value(ActionID::kSrtSize));
    tempAction->setCheckable(true);
    predicateAction[ActionID::kSrtSize] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kSrtSize));

    tempAction = subMenu->addAction(predicateName.value(ActionID::kSrtType));
    tempAction->setCheckable(true);
    predicateAction[ActionID::kSrtType] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kSrtType));

    return subMenu;
}

QMenu *SortAndDisplayMenuScenePrivate::addDisplayAsActions(QMenu *menu)
{
    QMenu *subMenu = new QMenu(menu);

    // DisplayAs
    QAction *tempAction = subMenu->addAction(predicateName.value(ActionID::kDisplayIcon));
    tempAction->setCheckable(true);
    predicateAction[ActionID::kDisplayIcon] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kDisplayIcon));

    tempAction = subMenu->addAction(predicateName.value(ActionID::kDisplayList));
    tempAction->setCheckable(true);
    predicateAction[ActionID::kDisplayList] = tempAction;
    tempAction->setProperty(ActionPropertyKey::kActionID, QString(ActionID::kDisplayList));

    return subMenu;
}

void SortAndDisplayMenuScenePrivate::sortByRole(int role)
{
    auto itemRole = static_cast<Global::ItemRoles>(role);
    Qt::SortOrder order = view->model()->sortOrder();
    int column = view->model()->getColumnByRole(itemRole);

    view->model()->setSortRole(itemRole);
    view->model()->sort(column, order);
}

void SortAndDisplayMenuScenePrivate::updateEmptyAreaActionState()
{
    using namespace Global;
    // sort  by
    auto role = static_cast<ItemRoles>(view->model()->sortRole());
    switch (role) {
    case kItemNameRole:
        predicateAction[ActionID::kSrtName]->setChecked(true);
        break;
    case kItemFileLastModifiedRole:
        predicateAction[ActionID::kSrtTimeModified]->setChecked(true);
        break;
    case kItemFileSizeRole:
        predicateAction[ActionID::kSrtSize]->setChecked(true);
        break;
    case kItemFileMimeTypeRole:
        predicateAction[ActionID::kSrtType]->setChecked(true);
        break;
    default:
        break;
    }

    // display as
    auto mode = view->currentViewMode();
    switch (mode) {
    case Global::ViewMode::kIconMode:
        predicateAction[ActionID::kDisplayIcon]->setChecked(true);
        break;
    case Global::ViewMode::kListMode:
        predicateAction[ActionID::kDisplayList]->setChecked(true);
        break;
    default:
        break;
    }
}