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
#ifndef PREVIEWFILEOPERATION_H
#define PREVIEWFILEOPERATION_H
#include "dfmplugin_filepreview_global.h"

#include <QObject>

DPFILEPREVIEW_BEGIN_NAMESPACE
class PreviewFileOperation : public QObject
{
    Q_OBJECT
public:
    explicit PreviewFileOperation(QObject *parent = nullptr);

public:
    static void openFileHandle(quint64 winID, const QUrl &url);
};
DPFILEPREVIEW_END_NAMESPACE
#endif   // PREVIEWFILEOPERATION_H