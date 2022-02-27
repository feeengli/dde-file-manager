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
#include "pluginloader_p.h"

#include <QApplication>
#include <QDir>
#include <QDebug>

DPFILEPREVIEW_USE_NAMESPACE
QStringList PreviewPluginLoaderPrivate::pluginPaths;
PreviewPluginLoaderPrivate::PreviewPluginLoaderPrivate(QObject *parent)
    : QObject(parent)
{
    if (pluginPaths.isEmpty()) {
        QString pluginsDir(qApp->applicationDirPath() + "/../../plugins/common/dfmplugin-preview");
        qInfo() << pluginsDir;
        if (!QDir(pluginsDir).exists()) {
            if (QT_PREPEND_NAMESPACE(qEnvironmentVariableIsEmpty)("DFM_PLUGIN_PATH"))
                pluginPaths.append(QString::fromLocal8Bit(PLUGINDIR).split(':'));
        } else {
            pluginPaths.append(pluginsDir);
        }
    }
}

PreviewPluginLoaderPrivate::~PreviewPluginLoaderPrivate()
{
}