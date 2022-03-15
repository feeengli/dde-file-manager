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
#include "smbsharefilemenu.h"

#include <QMenu>

DPSMBBROWSER_USE_NAMESPACE
DFMBASE_USE_NAMESPACE

SmbShareFileMenu::SmbShareFileMenu(QObject *parent)
    : AbstractMenu(parent)
{
}

QMenu *SmbShareFileMenu::build(QWidget *parent, dfmbase::AbstractMenu::MenuMode mode, const QUrl &rootUrl, const QUrl &focusUrl, const QList<QUrl> &selected, QVariant customData)
{
    QMenu *menu = new QMenu(parent);
    return menu;
}

void SmbShareFileMenu::actionBusiness(QAction *act)
{
}