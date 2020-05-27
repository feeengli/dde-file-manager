/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *               2016 ~ 2018 dragondjf
 *
 * Author:     dragondjf<dingjiangfeng@deepin.com>
 *
 * Maintainer: dragondjf<dingjiangfeng@deepin.com>
 *             zccrs<zhangjide@deepin.com>
 *             Tangtong<tangtong@deepin.com>
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

#include "computerview.h"
#include "dfilemenu.h"
#include "dfilemenumanager.h"
#include "windowmanager.h"
#include "dfmevent.h"
#include "dfmeventdispatcher.h"
#include "dfmapplication.h"
#include "dfmsettings.h"
#include "controllers/dfmsidebarvaultitemhandler.h"

#include "app/define.h"
#include "app/filesignalmanager.h"
#include "dfileservices.h"
#include "controllers/pathmanager.h"
#include "controllers/appcontroller.h"
#include "deviceinfo/udisklistener.h"
#include "dabstractfileinfo.h"
#include "interfaces/dfmstandardpaths.h"
#include "gvfs/gvfsmountmanager.h"
#include "singleton.h"
#include "../shutil/fileutils.h"
#include "dabstractfilewatcher.h"
#include "models/dfmrootfileinfo.h"
#include "models/computermodel.h"
#include "computerviewitemdelegate.h"
#include "views/dfmopticalmediawidget.h"
#include "models/deviceinfoparser.h"
#include "dfmvaultunlockpages.h"
#include "vault/interfaceactivevault.h"
#include "controllers/vaultcontroller.h"
#include "views/dfmvaultactiveview.h"

#include <dslider.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QTextEdit>
#include <QSizePolicy>
#include <QFile>
#include <QStorageInfo>
#include <QSettings>
#include <QUrlQuery>
#include <QScroller>

#include <private/qguiapplication_p.h>
#include <qpa/qplatformtheme.h>

#include <DApplication>
#include <DLineEdit>
#include <DStyle>

DWIDGET_USE_NAMESPACE

const QList<int> ComputerView::iconsizes = {48, 64, 96, 128, 256};

class ViewReturnEater : public QObject
{
    Q_OBJECT
public:
    explicit ViewReturnEater(QListView *parent) : QObject(parent){}

protected:
    bool eventFilter(QObject *, QEvent *e)
    {
        if (e->type() == QEvent::Type::KeyPress) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key::Key_Return || ke->key() == Qt::Key::Key_Enter) {
                QListView *v = qobject_cast<QListView*>(parent());
                Q_ASSERT(v);
                Q_EMIT entered(v->selectionModel()->currentIndex());
                return true;
            }
        }
        return false;
    }

Q_SIGNALS:
    void entered(const QModelIndex &idx);
};

ComputerView::ComputerView(QWidget *parent) : QWidget(parent)
{
    m_view = new QListView(this);
    m_statusbar = new DStatusBar(this);
    m_statusbar->scalingSlider()->setMaximum(iconsizes.count() - 1);
    m_statusbar->scalingSlider()->setMinimum(0);
    m_statusbar->scalingSlider()->setTickInterval(1);
    m_statusbar->scalingSlider()->setPageStep(1);
    m_statusbar->scalingSlider()->hide();
    m_statusbar->setMaximumHeight(22);

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_view);
    layout()->addWidget(m_statusbar);
    layout()->setMargin(0);

    m_model = new ComputerModel(this);
    m_view->setModel(m_model);
    m_view->setItemDelegate(new ComputerViewItemDelegate(this));
    m_view->setWrapping(true);
    m_view->setSpacing(10);
    m_view->setFlow(QListView::Flow::LeftToRight);
    m_view->setResizeMode(QListView::ResizeMode::Adjust);
    m_view->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    m_view->setIconSize(QSize(iconsizes[m_statusbar->scalingSlider()->value()], iconsizes[m_statusbar->scalingSlider()->value()]));
    m_view->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    m_view->setFrameShape(QFrame::Shape::NoFrame);
    m_view->installEventFilter(this);
    m_view->viewport()->installEventFilter(this);
    m_view->viewport()->setAutoFillBackground(false);

    DFMEvent event(this);
    event.setWindowId(window()->internalWinId());
    m_statusbar->itemCounted(event, m_model->itemCount());

    connect(&DFMVaultActiveView::getInstance(), &DFMVaultActiveView::accepted, this, [this](){
        // todo 进入保险箱主界面
        DUrl vaultUrl = VaultController::makeVaultUrl();
        appController->actionOpen(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << vaultUrl));
    });

    connect(m_model, &ComputerModel::itemCountChanged, this, [this](int count) {
        DFMEvent event(this);
        event.setWindowId(this->window()->internalWinId());
        if (this->m_view->selectionModel()->currentIndex().isValid()) {
            return;
        }
        this->m_statusbar->itemCounted(event, count);
    });
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this] {
        DFMEvent event(this);
        event.setWindowId(this->window()->internalWinId());
        if (this->m_view->selectionModel()->hasSelection()) {
            QModelIndex curidx(this->m_view->selectionModel()->currentIndex());
            DAbstractFileInfoPointer fi = fileService->createFileInfo(this, curidx.data(ComputerModel::DataRoles::DFMRootUrlRole).value<DUrl>());
            if (fi && fi->suffix() == SUFFIX_USRDIR) {
                DUrlList urlList{curidx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>()};
                event.setData(urlList);
            }
            this->m_statusbar->itemSelected(event, 1);
            return;
        }
        this->m_statusbar->itemCounted(event, this->m_model->itemCount());
    });

    connect(m_view, &QWidget::customContextMenuRequested, this, &ComputerView::contextMenu);
    auto enterfunc = [this](const QModelIndex &idx, int triggermatch) {
        if (~triggermatch) {
            if (DFMApplication::instance()->appAttribute(DFMApplication::AA_OpenFileMode).toInt() != triggermatch) {
                return;
            }
        }
        DUrl url = idx.data(ComputerModel::DataRoles::DFMRootUrlRole).value<DUrl>();
        //判断网络文件是否可以到达
        if (!DFileService::instance()->checkGvfsMountfileBusy(url)) {
            return;
        }
        if (url.path().endsWith(SUFFIX_USRDIR)) {
            appController->actionOpen(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << idx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>()));
        } else if (url.scheme() == DFMVAULT_SCHEME) { // 保险柜
            // 判断保险箱状态
            InterfaceActiveVault activeVault;
            EN_VaultState enState = activeVault.vaultState();
            switch (enState) {
            case EN_VaultState::NotAvailable:{  // 没有安装cryfs
                qDebug() << "Don't setup cryfs, can't use vault, please setup cryfs!";
                break;
            }
            case EN_VaultState::NotExisted:{    // 没有创建过保险箱，此时创建保险箱,创建成功后，进入主界面
                DFMVaultActiveView::getInstance().showTop();
                break;
            }
            case EN_VaultState::Encrypted:{ // 保险箱处于加密状态，弹出开锁对话框,开锁成功后，进入主界面
                // todo
                if(QDialog::Accepted == DFMVaultUnlockPages::instance()->exec()){
                    // 进入保险箱
                    appController->actionOpen(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << idx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>()));
                }
                break;
            }
            case EN_VaultState::Unlocked:{  // 保险箱处于开锁状态，直接进入主界面
                appController->actionOpen(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << idx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>()));
                break;
            }
            default:{   // 未考虑
                break;
            }
            }

        } else {
            appController->actionOpenDisk(dMakeEventPointer<DFMUrlBaseEvent>(this, url));
        }
    };

    QAction *newTabAction = new QAction();
    m_view->addAction(newTabAction);
    newTabAction->setShortcut(QKeySequence(Qt::Key::Key_T | Qt::Modifier::CTRL));
    connect(newTabAction, &QAction::triggered, [this] {
        if (m_view->selectionModel()->hasSelection()) {
            const QModelIndex &idx = m_view->selectionModel()->currentIndex();
            DUrl url = idx.data(ComputerModel::DataRoles::DFMRootUrlRole).value<DUrl>();
            if (url.path().endsWith(SUFFIX_USRDIR)) {
                appController->actionOpenInNewTab(dMakeEventPointer<DFMUrlBaseEvent>(this, idx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>()));
            } else {
                appController->actionOpenDiskInNewTab(dMakeEventPointer<DFMUrlBaseEvent>(this, url));
            }
        } else {
            appController->actionOpenInNewTab(dMakeEventPointer<DFMUrlBaseEvent>(this, rootUrl()));
        }
    });

    QAction *newWindowAction = new QAction();
    m_view->addAction(newWindowAction);
    newWindowAction->setShortcut(QKeySequence(Qt::Key::Key_N | Qt::Modifier::CTRL));
    connect(newWindowAction, &QAction::triggered, [this] {
        if (m_view->selectionModel()->hasSelection()) {
            const QModelIndex &idx = m_view->selectionModel()->currentIndex();
            DUrl url = idx.data(ComputerModel::DataRoles::DFMRootUrlRole).value<DUrl>();
            if (url.path().endsWith(SUFFIX_USRDIR)) {
                appController->actionOpenInNewWindow(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << idx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>()));
            } else {
                appController->actionOpenDiskInNewWindow(dMakeEventPointer<DFMUrlBaseEvent>(this, url));
            }
        } else {
            appController->actionOpenInNewWindow(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << rootUrl()));
        }
    });

    QAction *propAction = new QAction();
    m_view->addAction(propAction);
    propAction->setShortcut(QKeySequence(Qt::Key::Key_I | Qt::Modifier::CTRL));
    connect(propAction, &QAction::triggered, [this] {
        if (m_view->selectionModel()->hasSelection()) {
            const QModelIndex &idx = m_view->selectionModel()->currentIndex();
            DUrl url = idx.data(ComputerModel::DataRoles::DFMRootUrlRole).value<DUrl>();
            if (url.path().endsWith(SUFFIX_USRDIR)) {
                appController->actionProperty(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << idx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>()));
            } else {
                appController->actionProperty(dMakeEventPointer<DFMUrlListBaseEvent>(this, DUrlList() << url));
            }
        }
    });

    ViewReturnEater *re = new ViewReturnEater(m_view);
    m_view->installEventFilter(re);
    connect(m_view, &QAbstractItemView::doubleClicked, std::bind(enterfunc, std::placeholders::_1, 1));
    connect(m_view, &QAbstractItemView::clicked, std::bind(enterfunc, std::placeholders::_1, 0));
    connect(re, &ViewReturnEater::entered, std::bind(enterfunc, std::placeholders::_1, -1));
    connect(m_statusbar->scalingSlider(), &QSlider::valueChanged, this, [this] {m_view->setIconSize(QSize(iconsizes[m_statusbar->scalingSlider()->value()], iconsizes[m_statusbar->scalingSlider()->value()]));});
    connect(fileSignalManager, &FileSignalManager::requestRename, this, &ComputerView::onRenameRequested);

    connect(&DeviceInfoParser::Instance(), SIGNAL(loadFinished()), this, SLOT(repaint()));

    //! 用于保险箱大小更新
    connect(VaultController::getVaultController(), &VaultController::signalCalculationVaultFinish, this, &ComputerView::vaultCalculationFinish);
}

ComputerView::~ComputerView()
{
    ComputerModel *m = static_cast<ComputerModel*>(m_view->model());
    m_view->setModel(nullptr);
    delete m;
}

QWidget* ComputerView::widget() const
{
    return const_cast<ComputerView*>(this);
}

DUrl ComputerView::rootUrl() const
{
    return DUrl(COMPUTER_ROOT);
}

bool ComputerView::setRootUrl(const DUrl &url)
{
    return url == DUrl(COMPUTER_ROOT);
}

QListView* ComputerView::view()
{
    return m_view;
}

void ComputerView::contextMenu(const QPoint &pos)
{
    const QModelIndex &idx = m_view->indexAt(pos);
    if (!idx.isValid()) {
        return;
    }
    const QVector<MenuAction> &av = idx.data(ComputerModel::DataRoles::ActionVectorRole).value<QVector<MenuAction>>();
    QSet<MenuAction> disabled;
    if (!WindowManager::tabAddableByWinId(WindowManager::getWindowId(this))) {
        disabled.insert(MenuAction::OpenInNewTab);
        disabled.insert(MenuAction::OpenDiskInNewTab);
    }

    const QString &strVolTag = idx.data(ComputerModel::DataRoles::VolumeTagRole).value<QString>();
    if (strVolTag.startsWith("sr") // fix bug#25921 仅针对光驱设备实行禁用操作
            && !idx.data(ComputerModel::DataRoles::OpenUrlRole).value<DUrl>().isValid()) {
        //fix:光驱还没有加载成功前，右键点击光驱“挂载”，光驱自动弹出。
        disabled.insert(MenuAction::OpenDiskInNewWindow);
        disabled.insert(MenuAction::OpenDiskInNewTab);
        disabled.insert(MenuAction::Mount);
        //fix:不插光盘，打开文件管理器，光盘的弹出按钮不能置灰
        //disabled.insert(MenuAction::Eject);
        disabled.insert(MenuAction::SafelyRemoveDrive);

        disabled.insert(MenuAction::Property);
    }
    if (strVolTag.startsWith("sr") && DFMOpticalMediaWidget::g_mapCdStatusInfo[strVolTag].bBurningOrErasing) { // 如果当前光驱设备正在执行擦除/刻录，则禁用所有右键菜单选项
        for (MenuAction act: av)
            disabled.insert(act);
    }


    DFileMenu *menu = nullptr;
    if (idx.data(ComputerModel::DataRoles::SchemeRole) == DFMVAULT_SCHEME) {
        // 重新创建右键菜单
        DFMSideBarVaultItemHandler handler;
        quint64 wndId = WindowManager::getWindowId(this);
        menu = handler.generateMenu(WindowManager::getWindowById(wndId));
    } else {
        menu = DFileMenuManager::genereteMenuByKeys(av, disabled);
    }

    menu->setEventData(DUrl(), {idx.data(ComputerModel::DataRoles::DFMRootUrlRole).value<DUrl>()}, WindowManager::getWindowId(this), this);
    menu->exec(this->mapToGlobal(pos));
    menu->deleteLater(this);
}

void ComputerView::onRenameRequested(const DFMUrlBaseEvent &event)
{
    if (event.sender() != this) {
        return;
    }
    const QModelIndex &idx = m_model->findIndex(event.url());
    if (idx.isValid()) {
        m_view->edit(idx);
    }
}

void ComputerView::vaultCalculationFinish() const
{
    ComputerView * view = (ComputerView*)this;
    view->repaint();
}

void ComputerView::resizeEvent(QResizeEvent *event)
{
    for (int i = 0; i < m_view->model()->rowCount(); ++i) {
        if (m_view->model()->index(i, 0).data(ComputerModel::DataRoles::ICategoryRole) == ComputerModelItemData::Category::cat_splitter)
            emit m_view->itemDelegate()->sizeHintChanged(m_view->model()->index(i, 0));
    }
    QWidget::resizeEvent(event);
}

bool ComputerView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease && obj == m_view->viewport()) {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        const QModelIndex &idx = m_view->indexAt(e->pos());
        if (e->button() == Qt::MouseButton::LeftButton && (!idx.isValid() || !(idx.flags() & Qt::ItemFlag::ItemIsEnabled))) {
            m_view->selectionModel()->clearSelection();
        }
        return false;
    } else if (event->type() == QEvent::KeyPress && obj == m_view) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (ke->modifiers() == Qt::Modifier::ALT)
        {
            this->event(event);
            return true;
        }
        return false;
    } else {
        return QObject::eventFilter(obj, event);
    }
}

#include "computerview.moc"
