/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liqiang<liqianga@uniontech.com>
 *
 * Maintainer: liqiang<liqianga@uniontech.com>
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
#ifndef CANVASMENUSCENE_P_H
#define CANVASMENUSCENE_P_H
#include "canvasmenuscene.h"
#include "interfaces/private/abstractmenuscene_p.h"

#include <services/common/dfm_common_service_global.h>
#include <services/common/menu/menuservice.h>

#include <QMap>
#include <QMultiHash>

DSC_USE_NAMESPACE
DDP_CANVAS_BEGIN_NAMESPACE

class CanvasView;
class CanvasMenuScenePrivate : public dfmbase::AbstractMenuScenePrivate
{
public:
    explicit CanvasMenuScenePrivate(CanvasMenuScene *qq);

    void filterDisableAction(QMenu *menu);

public:
    QMap<QAction *, int> iconSizeAction;
    QPoint gridPos;

    CanvasView *view { nullptr };
    MenuService *menuServer = nullptr;

    QMultiHash<QString, QString> emptyDisableActions;
    QMultiHash<QString, QString> normalDisableActions;

private:
    CanvasMenuScene *q;
};

DDP_CANVAS_END_NAMESPACE
#endif   // CANVASMENUSCENE_P_H