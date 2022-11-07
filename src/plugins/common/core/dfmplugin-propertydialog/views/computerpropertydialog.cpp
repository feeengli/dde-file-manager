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

#include "computerpropertydialog.h"
#include "dfm-base/utils/universalutils.h"

#include <DSysInfo>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QFile>
#include <QDBusInterface>
#include <DFrame>

#include <cmath>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DFMBASE_USE_NAMESPACE
using namespace dfmplugin_propertydialog;
ComputerPropertyDialog::ComputerPropertyDialog(QWidget *parent)
    : DDialog(parent)
{
    iniUI();
    iniThread();
}

ComputerPropertyDialog::~ComputerPropertyDialog()
{
    if (thread) {
        if (thread->isRunning()) {
            thread->stopThread();
        }
        thread->quit();
        thread->deleteLater();
    }
}

void ComputerPropertyDialog::iniUI()
{
    setTitle(tr("Computer"));

    computerIcon = new DLabel(this);
    QString distributerLogoPath = DSysInfo::distributionOrgLogo();
    QIcon logoIcon;
    if (!distributerLogoPath.isEmpty() && QFile::exists(distributerLogoPath)) {
        logoIcon = QIcon(distributerLogoPath);
    } else {
        logoIcon = QIcon::fromTheme("dfm_deepin_logo");
    }
    computerIcon->setPixmap(logoIcon.pixmap(152, 39));

    basicInfo = new DLabel(tr("Basic Info"), this);
    DFontSizeManager::instance()->bind(basicInfo, DFontSizeManager::T5, QFont::Medium);
    basicInfo->setForegroundRole(DPalette::TextTitle);
    basicInfo->setAlignment(Qt::AlignLeft);

    computerName = new KeyValueLabel(this);
    computerName->setLeftValue(tr("Computer name"));
    computerVersionNum = new KeyValueLabel(this);
    computerVersionNum->setLeftValue(tr("Version"));
    computerEdition = new KeyValueLabel(this);
    computerEdition->setLeftValue(tr("Edition"));
    computerOSBuild = new KeyValueLabel(this);
    computerOSBuild->setLeftValue(tr("OS build"));
    computerType = new KeyValueLabel(this);
    computerType->setLeftValue(tr("Type"));
    computerCpu = new KeyValueLabel(this);
    computerCpu->setLeftValue(tr("Processor"), Qt::ElideNone, Qt::AlignLeft | Qt::AlignVCenter);
    computerMemory = new KeyValueLabel(this);
    computerMemory->setLeftValue(tr("Memory"));
    computerMemory->setRightFontSizeWeight(DFontSizeManager::T9);

    DFrame *basicInfoFrame = new DFrame(this);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(10, 2, 10, 5);
    vlayout->setSpacing(8);
    vlayout->addWidget(basicInfo);
    vlayout->addWidget(computerName);
    vlayout->addWidget(computerVersionNum);
    vlayout->addWidget(computerEdition);
    vlayout->addWidget(computerOSBuild);
    vlayout->addWidget(computerType);
    vlayout->addWidget(computerCpu);
    vlayout->addWidget(computerMemory);
    basicInfoFrame->setLayout(vlayout);

    QFrame *contentFrame = new QFrame(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addSpacing(25);
    mainLayout->addWidget(computerIcon, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(basicInfoFrame);

    contentFrame->setLayout(mainLayout);

    setFixedWidth(320);

    addContent(contentFrame);
}

void ComputerPropertyDialog::iniThread()
{
    thread = new ComputerInfoThread();
    qRegisterMetaType<QMap<ComputerInfoItem, QString>>("QMap<ComputerInfoItem, QString>");
    connect(thread, &ComputerInfoThread::sigSendComputerInfo, this, &ComputerPropertyDialog::computerProcess);
}

/*!
 * \brief                   接收线程发来的获取到计算机的信息，并对信息进行设置到相应控件中
 * \param computerInfo      接收到的计算机信息
 */
void ComputerPropertyDialog::computerProcess(QMap<ComputerInfoItem, QString> computerInfo)
{
    if (computerInfo.contains(ComputerInfoItem::kName))
        computerName->setRightValue(computerInfo[ComputerInfoItem::kName]);
    if (computerInfo.contains(ComputerInfoItem::kVersion))
        computerVersionNum->setRightValue(computerInfo[ComputerInfoItem::kVersion]);
    if (computerInfo.contains(ComputerInfoItem::kEdition))
        computerEdition->setRightValue(computerInfo[ComputerInfoItem::kEdition]);
    if (computerInfo.contains(ComputerInfoItem::kOSBuild))
        computerOSBuild->setRightValue(computerInfo[ComputerInfoItem::kOSBuild]);
    if (computerInfo.contains(ComputerInfoItem::kType))
        computerType->setRightValue(computerInfo[ComputerInfoItem::kType]);
    if (computerInfo.contains(ComputerInfoItem::kCpu)) {
        computerCpu->setRightValue(computerInfo[ComputerInfoItem::kCpu]);
        computerCpu->setRightWordWrap(true);
        QFontMetrics fontMetrics(computerCpu->font());
        QRect boundingRect = fontMetrics.boundingRect(computerInfo[ComputerInfoItem::kCpu]);
        computerCpu->setRowMinimumHeight(boundingRect.height() - boundingRect.y());
    }
    if (computerInfo.contains(ComputerInfoItem::kMemory))
        computerMemory->setRightValue(computerInfo[ComputerInfoItem::kMemory]);
}

void ComputerPropertyDialog::showEvent(QShowEvent *event)
{
    thread->startThread();
    DDialog::showEvent(event);
}

void ComputerPropertyDialog::closeEvent(QCloseEvent *event)
{
    thread->stopThread();
    DDialog::closeEvent(event);
}

static QString formatCap(qulonglong cap, const int size = 1024, quint8 precision = 1)
{
    static QString type[] = { " B", " KB", " MB", " GB", " TB" };

    qulonglong lc = cap;
    double dc = cap;
    double ds = size;

    for (size_t p = 0; p < sizeof(type); ++p) {
        if (cap < pow(size, p + 1) || p == sizeof(type) - 1) {
            if (!precision) {
                //! 内存总大小只能是整数所以当内存大小有小数时，就需要向上取整
                int mem = static_cast<int>(ceil(lc / pow(size, p)));
#ifdef __sw_64__
                return QString::number(mem) + type[p];
#else
                //! 如果向上取整后内存大小不为偶数，就向下取整
                if (mem % 2 > 0)
                    mem = static_cast<int>(floor(lc / pow(size, p)));
                return QString::number(mem) + type[p];
#endif
            }

            return QString::number(dc / pow(ds, p), 'f', precision) + type[p];
        }
    }

    return "";
}

ComputerInfoThread::ComputerInfoThread(QObject *parent)
    : QThread(parent)
{
}

ComputerInfoThread::~ComputerInfoThread()
{
}

void ComputerInfoThread::startThread()
{
    computerData.clear();
    threadStop = false;
    start();
}

void ComputerInfoThread::stopThread()
{
    threadStop = true;
}

void ComputerInfoThread::run()
{
    while (computerData.size() <= 1 && !threadStop) {
        computerProcess();
        sleep(1);
    }
}

/*!
 * \brief 通过DBus获取计算机信息，并保存在computerData中，
 *        判断computerData中是否数据量是大于1就把computerData通过信号发送给计算机页面
 */
void ComputerInfoThread::computerProcess()
{
    computerData.insert(ComputerInfoItem::kName, computerName());
    computerData.insert(ComputerInfoItem::kVersion, versionNum());
    computerData.insert(ComputerInfoItem::kEdition, edition());
    computerData.insert(ComputerInfoItem::kOSBuild, osBuild());
    computerData.insert(ComputerInfoItem::kType, systemType());
    computerData.insert(ComputerInfoItem::kCpu, cpuInfo());
    computerData.insert(ComputerInfoItem::kMemory, memoryInfo());
    emit sigSendComputerInfo(computerData);
}

QString ComputerInfoThread::computerName() const
{
    return DSysInfo::computerName();
}

QString ComputerInfoThread::versionNum() const
{
    return DSysInfo::majorVersion();
}

QString ComputerInfoThread::edition() const
{
    if (DSysInfo::uosType() == DSysInfo::UosServer || DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        return QString("%1%2").arg(DSysInfo::minorVersion()).arg(DSysInfo::uosEditionName());
    } else if (DSysInfo::isDeepin) {
        return QString("%1(%2)").arg(DSysInfo::uosEditionName()).arg(DSysInfo::minorVersion());
    } else {
        return QString("%1 %2").arg(DSysInfo::productVersion()).arg(DSysInfo::productTypeString());
    }
}

QString ComputerInfoThread::osBuild() const
{
    return DSysInfo::buildVersion();
}

QString ComputerInfoThread::systemType() const
{
    return QString::number(QSysInfo::WordSize) + tr("Bit");
}

QString ComputerInfoThread::cpuInfo() const
{
    if (DSysInfo::cpuModelName().contains("Hz")) {
        return DSysInfo::cpuModelName();
    } else {
        QDBusInterface interface("com.deepin.daemon.SystemInfo",
                                 "/com/deepin/daemon/SystemInfo",
                                 "org.freedesktop.DBus.Properties",
                                 QDBusConnection::sessionBus());
        interface.setTimeout(1000);
        if (!interface.isValid()) {
            qWarning() << "Dbus com.deepin.daemon.SystemInfo is not valid!";
            return "";
        }
        qInfo() << "Start call Dbus com.deepin.daemon.SystemInfo CPUMaxMHz";
        QDBusMessage reply = interface.call("Get", "com.deepin.daemon.SystemInfo", "CPUMaxMHz");
        qInfo() << "End call Dbus com.deepin.daemon.SystemInfo CPUMaxMHz";
        QList<QVariant> outArgs = reply.arguments();
        double cpuMaxMhz = outArgs.at(0).value<QDBusVariant>().variant().toDouble();

        if (DSysInfo::cpuModelName().isEmpty()) {
            qInfo() << "Start call Dbus com.deepin.daemon.SystemInfo Processor";
            QDBusMessage replyCpuInfo = interface.call("Get", "com.deepin.daemon.SystemInfo", "Processor");
            qInfo() << "End call Dbus com.deepin.daemon.SystemInfo Processor";
            QList<QVariant> outArgsCpuInfo = replyCpuInfo.arguments();
            QString processor = outArgsCpuInfo.at(0).value<QDBusVariant>().variant().toString();
            return QString("%1 @ %2GHz").arg(processor).arg(cpuMaxMhz / 1000);
        } else {
            return QString("%1 @ %2GHz").arg(DSysInfo::cpuModelName()).arg(cpuMaxMhz / 1000);
        }
    }
}

QString ComputerInfoThread::memoryInfo() const
{
    quint64 memoryAvailableSize = static_cast<quint64>(DSysInfo::memoryTotalSize());
    quint64 memoryInstalledSize;
    QDBusInterface interface("com.deepin.system.SystemInfo",
                             "/com/deepin/system/SystemInfo",
                             "com.deepin.system.SystemInfo",
                             QDBusConnection::systemBus());
    interface.setTimeout(1000);
    if (interface.isValid()) {
        qInfo() << "Start call com.deepin.system.SystemInfo MemorySize";
        memoryInstalledSize = interface.property("MemorySize").toULongLong();
        qInfo() << "End call Dbus com.deepin.system.SystemInfo MemorySize";
        qInfo() << "Get memoryInstalledSize from dbus!";
    } else {
        memoryInstalledSize = static_cast<quint64>(DSysInfo::memoryInstalledSize());
        qInfo() << "Get memoryInstalledSize from dtk!";
    }
    return QString("%1 (%2 %3)")
            .arg(formatCap(memoryInstalledSize, 1024, 0))
            .arg(formatCap(memoryAvailableSize))
            .arg(tr("Available"));
}