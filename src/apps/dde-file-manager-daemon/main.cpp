/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangsheng<zhangsheng@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             xushitong<xushitong@uniontech.com>
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
#include "config.h"   // cmake

#include <QCoreApplication>

#include <QDebug>
#include <QDir>

#include <dfm-framework/dpf.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>

static constexpr char kDaemonInterface[] { "org.deepin.plugin.daemon" };
static constexpr char kPluginCore[] { "daemonplugin-core" };
static constexpr char kLibCore[] { "libdaemonplugin-core.so" };

static void initEnv()
{
    ///###: why?
    ///###: when dbus invoke a daemon the variants in the environment of daemon(s) are empty.
    ///###: So we need to set them.
    if (!qEnvironmentVariableIsSet("LANG")) {
        qputenv("LANG", "en_US.UTF8");
    }

    if (!qEnvironmentVariableIsSet("LANGUAGE")) {
        qputenv("LANGUAGE", "en_US");
    }

    if (!qEnvironmentVariableIsSet("HOME")) {
        qputenv("HOME", getpwuid(getuid())->pw_dir);
    }
}

static void initLog()
{

    QString logPath = "/var/log/" + QCoreApplication::organizationName() + QLatin1Char('/') + QCoreApplication::applicationName() + QLatin1Char('/');
    QDir logDir(logPath);
    if (!logDir.exists())
        QDir().mkpath(logPath);

    dpfLogManager->setlogFilePath(logPath + QCoreApplication::applicationName() + ".log");
    dpfLogManager->registerConsoleAppender();
    dpfLogManager->registerFileAppender();
}

static bool pluginsLoad()
{
    // set plugin iid from qt style
    DPF_NAMESPACE::LifeCycle::addPluginIID(kDaemonInterface);

    QDir dir(qApp->applicationDirPath());
    QString pluginsDir;
    if (!dir.cd("../../plugins/")) {
        qInfo() << QString("Path does not exist, use path : %1").arg(DFM_PLUGIN_PATH);
        pluginsDir = DFM_PLUGIN_PATH;
    } else {
        pluginsDir = dir.absolutePath();
    }
    qDebug() << "using plugins dir:" << pluginsDir;
    DPF_NAMESPACE::LifeCycle::setPluginPaths({ pluginsDir });

    qInfo() << "Depend library paths:" << QCoreApplication::libraryPaths();
    qInfo() << "Load plugin paths: " << dpf::LifeCycle::pluginPaths();

    // read all plugins in setting paths
    if (!DPF_NAMESPACE::LifeCycle::readPlugins())
        return false;

    // We should make sure that the core plugin is loaded first
    auto corePlugin = DPF_NAMESPACE::LifeCycle::pluginMetaObj(kPluginCore);
    if (corePlugin.isNull())
        return false;
    if (!corePlugin->fileName().contains(kLibCore)) {
        qWarning() << corePlugin->fileName() << "is not" << kLibCore;
        return false;
    }
    if (!DPF_NAMESPACE::LifeCycle::loadPlugin(corePlugin))
        return false;

    // load plugins without core
    if (!DPF_NAMESPACE::LifeCycle::loadPlugins())
        return false;

    return true;
}

int main(int argc, char *argv[])
{
    initEnv();
    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");

    DPF_NAMESPACE::backtrace::initbacktrace();

    initLog();
    if (!pluginsLoad()) {
        qCritical() << "Load pugin failed!";
        abort();
    }

    return a.exec();
}