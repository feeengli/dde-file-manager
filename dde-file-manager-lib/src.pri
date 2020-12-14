#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------
RESOURCES += \
    $$PWD/skin/skin.qrc \
    $$PWD/skin/filemanager.qrc \
    $$PWD/themes/themes.qrc \
    $$PWD/configure.qrc \
    $$PWD/resources/resources.qrc

HEADERS += \
    $$PWD/controllers/appcontroller.h \
    $$PWD/controllers/dfmrootcontroller.h \
    $$PWD/models/computermodel.h \
    $$PWD/models/dfmrootfileinfo.h \
    $$PWD/views/computerviewitemdelegate.h \
    $$PWD/views/dtoolbar.h \
    $$PWD/views/dfileview_p.h   \
    $$PWD/views/dfileview.h \
    $$PWD/views/ddetailview.h \
    $$PWD/controllers/filecontroller.h \
    $$PWD/app/filesignalmanager.h \
    $$PWD/views/fileitem.h \
    $$PWD/models/desktopfileinfo.h \
    $$PWD/models/bookmark.h \
    $$PWD/models/searchhistory.h \
    $$PWD/controllers/bookmarkmanager.h \
    $$PWD/dialogs/dialogmanager.h \
    $$PWD/controllers/searchhistroymanager.h \
    $$PWD/views/windowmanager.h \
    $$PWD/shutil/desktopfile.h \
    $$PWD/shutil/fileutils.h \
    $$PWD/shutil/properties.h \
    $$PWD/views/dfilemanagerwindow_p.h  \
    $$PWD/views/dfilemanagerwindow.h \
    $$PWD/views/historystack.h\
    $$PWD/dialogs/propertydialog.h \
    $$PWD/controllers/trashmanager.h \
    $$PWD/models/trashfileinfo.h \
    $$PWD/shutil/mimesappsmanager.h \
    $$PWD/dialogs/openwithdialog.h \
    $$PWD/controllers/searchcontroller.h \
    $$PWD/models/searchfileinfo.h\
    $$PWD/dialogs/basedialog.h \
    $$PWD/views/extendview.h \
    $$PWD/controllers/pathmanager.h \
    $$PWD/shutil/mimetypedisplaymanager.h \
    $$PWD/views/dstatusbar.h \
    $$PWD/controllers/subscriber.h \
    $$PWD/models/dfileselectionmodel.h \
    $$PWD/dialogs/closealldialogindicator.h \
    $$PWD/gvfs/mountaskpassworddialog.h \
    $$PWD/gvfs/networkmanager.h \
    $$PWD/gvfs/secretmanager.h \
    $$PWD/models/networkfileinfo.h \
    $$PWD/controllers/networkcontroller.h \
    $$PWD/dialogs/trashpropertydialog.h \
    $$PWD/controllers/jobcontroller.h \
    $$PWD/views/computerview.h \
    $$PWD/shutil/shortcut.h \
    $$PWD/views/dtabbar.h \
    $$PWD/views/dfiledialog.h \
    $$PWD/interfaces/dfiledialoghandle.h \
    $$PWD/dialogs/shareinfoframe.h \
    $$PWD/interfaces/dfmstandardpaths.h \
    $$PWD/interfaces/dfmglobal.h \
    $$PWD/controllers/sharecontroler.h \
    $$PWD/models/sharefileinfo.h \
    $$PWD/interfaces/dfileviewhelper.h \
    $$PWD/interfaces/diconitemdelegate.h \
    $$PWD/views/fileviewhelper.h \
    $$PWD/interfaces/dlistitemdelegate.h \
    $$PWD/interfaces/durl.h \
    $$PWD/interfaces/dfilemenu.h \
    $$PWD/interfaces/ddiriterator.h \
    $$PWD/interfaces/private/dstyleditemdelegate_p.h \
    $$PWD/interfaces/dfilesystemmodel.h \
    $$PWD/app/define.h \
    $$PWD/interfaces/dabstractfilecontroller.h \
    $$PWD/interfaces/dabstractfileinfo.h \
    $$PWD/interfaces/dfilemenumanager.h \
    $$PWD/interfaces/dfileservices.h \
    $$PWD/interfaces/dfmevent.h \
    $$PWD/dialogs/usersharepasswordsettingdialog.h\
    $$PWD/interfaces/private/dabstractfileinfo_p.h \
    $$PWD/interfaces/dfileinfo.h \
    $$PWD/interfaces/private/dfileinfo_p.h \
    $$PWD/interfaces/dfilesystemwatcher.h \
    $$PWD/interfaces/private/dfilesystemwatcher_p.h \
    $$PWD/interfaces/dabstractfilewatcher.h \
    $$PWD/interfaces/dfilewatcher.h \
    $$PWD/interfaces/private/dabstractfilewatcher_p.h \
    $$PWD/interfaces/dfileproxywatcher.h \
    $$PWD/plugins/pluginmanager.h \
    $$PWD/interfaces/dthumbnailprovider.h \
    $$PWD/controllers/avfsfilecontroller.h \
    $$PWD/models/avfsfileinfo.h \
    $$PWD/interfaces/dfileiconprovider.h \
    $$PWD/interfaces/dfilewatchermanager.h \
    $$PWD/dialogs/computerpropertydialog.h \
    $$PWD/interfaces/dmimedatabase.h \
    $$PWD/gvfs/qdrive.h \
    $$PWD/gvfs/qvolume.h \
    $$PWD/gvfs/qmount.h \
    $$PWD/gvfs/gvfsmountmanager.h \
    $$PWD/gvfs/qdiskinfo.h \
    $$PWD/interfaces/dfmeventdispatcher.h \
    $$PWD/interfaces/dfmabstracteventhandler.h \
    $$PWD/controllers/fileeventprocessor.h \
    $$PWD/interfaces/dfmbaseview.h \
    $$PWD/interfaces/dfmviewmanager.h \
    $$PWD/views/dfmactionbutton.h \
    $$PWD/interfaces/plugins/dfmviewplugin.h \
    $$PWD/interfaces/plugins/dfmviewfactory.h \
    $$PWD/interfaces/plugins/dfmfactoryloader.h \
    $$PWD/interfaces/plugins/dfmfilecontrollerplugin.h \
    $$PWD/interfaces/plugins/dfmfilecontrollerfactory.h \
    $$PWD/interfaces/plugins/dfmgenericplugin.h \
    $$PWD/interfaces/plugins/dfmgenericfactory.h \
    $$PWD/views/filedialogstatusbar.h \
    $$PWD/interfaces/plugins/dfmfilepreviewplugin.h \
    $$PWD/interfaces/plugins/dfmfilepreviewfactory.h \
    $$PWD/interfaces/dfmfilepreview.h \
    $$PWD/dialogs/filepreviewdialog.h \
    $$PWD/gvfs/mountsecretdiskaskpassworddialog.h \
    $$PWD/views/drenamebar.h \
    $$PWD/shutil/filebatchprocess.h \
    $$PWD/dialogs/ddesktoprenamedialog.h \
    $$PWD/dialogs/dmultifilepropertydialog.h \
    $$PWD/dialogs/movetotrashconflictdialog.h \
    $$PWD/dialogs/dfmsettingdialog.h \
    $$PWD/controllers/operatorrevocation.h \
    $$PWD/controllers/tagcontroller.h \
    $$PWD/models/tagfileinfo.h \
    $$PWD/views/dtagactionwidget.h \
    $$PWD/views/droundbutton.h \
    $$PWD/views/dtagedit.h \
    $$PWD/shutil/dsqlitehandle.h \
    $$PWD/shutil/danythingmonitorfilter.h \
    $$PWD/controllers/tagmanagerdaemoncontroller.h \
    $$PWD/controllers/interface/tagmanagerdaemon_interface.h \
    $$PWD/interfaces/dfmsettings.h \
    $$PWD/interfaces/dfmcrumbbar.h \
    $$PWD/interfaces/dfmcrumbinterface.h \
    $$PWD/interfaces/plugins/dfmcrumbplugin.h \
    $$PWD/interfaces/plugins/dfmcrumbfactory.h \
    $$PWD/interfaces/dfmcrumbmanager.h \
    $$PWD/controllers/dfmfilecrumbcontroller.h \
    $$PWD/controllers/dfmcomputercrumbcontroller.h \
    $$PWD/controllers/dfmbookmarkcrumbcontroller.h \
    $$PWD/controllers/dfmtagcrumbcontroller.h \
    $$PWD/controllers/dfmnetworkcrumbcontroller.h \
    $$PWD/controllers/dfmtrashcrumbcontroller.h \
    $$PWD/controllers/dfmusersharecrumbcontroller.h \
    $$PWD/controllers/dfmavfscrumbcontroller.h \
    $$PWD/controllers/dfmsmbcrumbcontroller.h \
    $$PWD/controllers/dfmmtpcrumbcontroller.h \
    $$PWD/views/dfmaddressbar.h \
    $$PWD/views/dcompleterlistview.h \
    $$PWD/interfaces/dfmapplication.h \
    $$PWD/interfaces/private/dfmapplication_p.h \
    $$PWD/controllers/dfmsearchcrumbcontroller.h \
    $$PWD/views/dfmheaderview.h \
    $$PWD/controllers/mountcontroller.h \
    $$PWD/models/mountfileinfo.h \
    $$PWD/models/recentfileinfo.h \
    $$PWD/controllers/recentcontroller.h \
    $$PWD/controllers/dfmrecentcrumbcontroller.h \
    $$PWD/views/dfmadvancesearchbar.h \
    $$PWD/shutil/dfmregularexpression.h \
    $$PWD/controllers/mergeddesktopcontroller.h \
    $$PWD/models/mergeddesktopfileinfo.h \
    $$PWD/controllers/dfmmdcrumbcontrooler.h \
    $$PWD/interfaces/private/mergeddesktop_common_p.h \
    $$PWD/controllers/masteredmediacontroller.h \
    $$PWD/controllers/dfmmasteredmediacrumbcontroller.h \
    $$PWD/models/masteredmediafileinfo.h \
    $$PWD/views/dfmopticalmediawidget.h \
    $$PWD/views/dfmrightdetailview.h \
    $$PWD/dialogs/burnoptdialog.h \
    $$PWD/interfaces/dfmcrumblistviewmodel.h \
    $$PWD/interfaces/dfmstyleditemdelegate.h \
    $$PWD/views/dfmsidebaritemdelegate.h \
    $$PWD/models/dfmsidebarmodel.h \
    $$PWD/views/dfmsidebarview.h \
    $$PWD/interfaces/dfmsidebariteminterface.h \
    $$PWD/interfaces/dfmsidebarmanager.h \
    $$PWD/controllers/dfmsidebardeviceitemhandler.h \
    $$PWD/controllers/dfmsidebartagitemhandler.h \
    $$PWD/controllers/dfmsidebardefaultitemhandler.h \
    $$PWD/controllers/dfmsidebarbookmarkitemhandler.h \
    $$PWD/interfaces/dfmsidebaritem.h \
    $$PWD/views/dfmsidebar.h \
    $$PWD/views/dfmfilebasicinfowidget.h \
    $$PWD/views/dfmtagwidget.h \
    $$PWD/controllers/vaultcontroller.h \
    $$PWD/models/vaultfileinfo.h \
    $$PWD/controllers/dfmvaultcrumbcontroller.h \
    $$PWD/views/dfmvaultunlockpages.h \
    $$PWD/views/dfmvaultfileview.h \
    $$PWD/views/dfmvaultrecoverykeypages.h \
    $$PWD/plugins/dfmadditionalmenu_p.h \
    $$PWD/plugins/dfmadditionalmenu.h \
    $$PWD/dialogs/connecttoserverdialog.h \
    $$PWD/shutil/dfmfilelistfile.h \
    $$PWD/views/dfmsplitter.h \
    $$PWD/dbus/dbussysteminfo.h \
    $$PWD/models/deviceinfoparser.h \
    $$PWD/controllers/dfmsidebarvaultitemhandler.h \
    $$PWD/controllers/vaulthandle.h \
    $$PWD/controllers/vaulterrorcode.h \
    $$PWD/views/dfmvaultremovepages.h \
    $$PWD/views/dfmvaultactiveview.h \
    $$PWD/views/dfmvaultactivestartview.h \
    $$PWD/views/dfmvaultactivesavekeyview.h \
    $$PWD/views/dfmvaultactivefinishedview.h \
    $$PWD/views/dfmvaultactivesetunlockmethodview.h \
    $$PWD/views/dfmvaultremoveprogressview.h \
    $$PWD/views/dfmvaultremovebypasswordview.h \
    $$PWD/views/dfmvaultremovebyrecoverykeyview.h \
    $$PWD/views/dfmvaultpagebase.h \
    $$PWD/bluetooth/bluetoothmanager.h \
    $$PWD/bluetooth/bluetoothadapter.h \
    $$PWD/bluetooth/bluetoothdevice.h \
    $$PWD/bluetooth/bluetoothmodel.h \
    $$PWD/bluetooth/bluetoothtransdialog.h \
    $$PWD/interfaces/drootfilemanager.h \
    $$PWD/interfaces/dgvfsfileinfo.h \
    $$PWD/interfaces/private/dgvfsfileinfo_p.h \
    $$PWD/shutil/checknetwork.h \
    $$PWD/accessibility/ac-lib-file-manager.h \
    $$PWD/accessibility/acintelfunctions.h \
    $$PWD/bluetooth/bluetoothmanager_p.h \
    $$PWD/models/virtualentryinfo.h \
    $$PWD/models/trashfileinfo_p.h \
    $$PWD/controllers/mergeddesktopcontroller_p.h \
    $$PWD/controllers/masteredmediacontroller_p.h \
    $$PWD/interfaces/plugins/private/dfmfactoryloader_p.h \
    $$PWD/interfaces/plugins/private/dfmfilepreviewfactory_p.h \
    $$PWD/dialogs/private/ddesktoprenamedialog_p.h \
    $$PWD/dialogs/private/dfmsettingdialog_p.h \
    $$PWD/interfaces/plugins/private/dfmviewfactory_p.h

SOURCES += \
    $$PWD/controllers/appcontroller.cpp \
    $$PWD/controllers/dfmrootcontroller.cpp \
    $$PWD/models/computermodel.cpp \
    $$PWD/models/dfmrootfileinfo.cpp \
    $$PWD/views/computerviewitemdelegate.cpp \
    $$PWD/views/dtoolbar.cpp \
    $$PWD/views/dfileview.cpp \
    $$PWD/views/ddetailview.cpp \
    $$PWD/controllers/filecontroller.cpp \
    $$PWD/views/fileitem.cpp \
    $$PWD/models/desktopfileinfo.cpp \
    $$PWD/models/bookmark.cpp \
    $$PWD/models/searchhistory.cpp \
    $$PWD/controllers/bookmarkmanager.cpp \
    $$PWD/dialogs/dialogmanager.cpp \
    $$PWD/controllers/searchhistroymanager.cpp \
    $$PWD/views/windowmanager.cpp \
    $$PWD/shutil/desktopfile.cpp \
    $$PWD/shutil/fileutils.cpp \
    $$PWD/shutil/properties.cpp \
    $$PWD/views/dfilemanagerwindow.cpp \
    $$PWD/views/historystack.cpp\
    $$PWD/dialogs/propertydialog.cpp \
    $$PWD/controllers/trashmanager.cpp \
    $$PWD/models/trashfileinfo.cpp \
    $$PWD/shutil/mimesappsmanager.cpp \
    $$PWD/dialogs/openwithdialog.cpp \
    $$PWD/controllers/searchcontroller.cpp \
    $$PWD/models/searchfileinfo.cpp\
    $$PWD/dialogs/basedialog.cpp \
    $$PWD/views/extendview.cpp \
    $$PWD/controllers/pathmanager.cpp \
    $$PWD/shutil/mimetypedisplaymanager.cpp \
    $$PWD/views/dstatusbar.cpp \
    $$PWD/controllers/subscriber.cpp \
    $$PWD/models/dfileselectionmodel.cpp \
    $$PWD/dialogs/closealldialogindicator.cpp \
    $$PWD/gvfs/mountaskpassworddialog.cpp \
    $$PWD/gvfs/networkmanager.cpp \
    $$PWD/gvfs/secretmanager.cpp \
    $$PWD/models/networkfileinfo.cpp \
    $$PWD/controllers/networkcontroller.cpp \
    $$PWD/dialogs/trashpropertydialog.cpp \
    $$PWD/controllers/jobcontroller.cpp \
    $$PWD/views/computerview.cpp \
    $$PWD/shutil/shortcut.cpp \
    $$PWD/views/dtabbar.cpp \
    $$PWD/views/dfiledialog.cpp \
    $$PWD/interfaces/dfiledialoghandle.cpp \
    $$PWD/dialogs/shareinfoframe.cpp \
    $$PWD/interfaces/dfmstandardpaths.cpp \
    $$PWD/interfaces/dfmglobal.cpp \
    $$PWD/controllers/sharecontroler.cpp \
    $$PWD/models/sharefileinfo.cpp \
    $$PWD/interfaces/dfileviewhelper.cpp \
    $$PWD/interfaces/diconitemdelegate.cpp \
    $$PWD/views/fileviewhelper.cpp \
    $$PWD/interfaces/dlistitemdelegate.cpp \
    $$PWD/interfaces/durl.cpp \
    $$PWD/interfaces/dfilemenu.cpp \
    $$PWD/interfaces/dfilesystemmodel.cpp \
    $$PWD/app/define.cpp \
    $$PWD/interfaces/dabstractfilecontroller.cpp \
    $$PWD/interfaces/dabstractfileinfo.cpp \
    $$PWD/interfaces/dfilemenumanager.cpp \
    $$PWD/interfaces/dfileservices.cpp \
    $$PWD/interfaces/dfmevent.cpp \
    $$PWD/interfaces/dfileinfo.cpp \
    $$PWD/interfaces/dfilesystemwatcher.cpp \
    $$PWD/interfaces/dabstractfilewatcher.cpp \
    $$PWD/interfaces/dfilewatcher.cpp \
    $$PWD/interfaces/dfileproxywatcher.cpp \
    $$PWD/app/filesignalmanager.cpp \
    $$PWD/plugins/pluginmanager.cpp \
    $$PWD/interfaces/dthumbnailprovider.cpp \
    $$PWD/controllers/avfsfilecontroller.cpp \
    $$PWD/models/avfsfileinfo.cpp \
    $$PWD/interfaces/dfileiconprovider.cpp \
    $$PWD/interfaces/dfilewatchermanager.cpp \
    $$PWD/dialogs/computerpropertydialog.cpp \
    $$PWD/interfaces/dmimedatabase.cpp \
    $$PWD/gvfs/qdrive.cpp \
    $$PWD/gvfs/qvolume.cpp \
    $$PWD/gvfs/qmount.cpp \
    $$PWD/gvfs/gvfsmountmanager.cpp \
    $$PWD/gvfs/qdiskinfo.cpp \
    $$PWD/interfaces/dfmeventdispatcher.cpp \
    $$PWD/interfaces/dfmabstracteventhandler.cpp \
    $$PWD/controllers/fileeventprocessor.cpp \
    $$PWD/interfaces/dfmbaseview.cpp \
    $$PWD/interfaces/dfmviewmanager.cpp \
    $$PWD/views/dfmactionbutton.cpp \
    $$PWD/interfaces/plugins/dfmviewplugin.cpp \
    $$PWD/interfaces/plugins/dfmviewfactory.cpp \
    $$PWD/interfaces/plugins/dfmfactoryloader.cpp \
    $$PWD/interfaces/plugins/dfmfilecontrollerplugin.cpp \
    $$PWD/interfaces/plugins/dfmfilecontrollerfactory.cpp \
    $$PWD/interfaces/plugins/dfmgenericplugin.cpp \
    $$PWD/interfaces/plugins/dfmgenericfactory.cpp \
    $$PWD/views/filedialogstatusbar.cpp \
    $$PWD/interfaces/plugins/dfmfilepreviewplugin.cpp \
    $$PWD/interfaces/plugins/dfmfilepreviewfactory.cpp \
    $$PWD/interfaces/dfmfilepreview.cpp \
    $$PWD/dialogs/filepreviewdialog.cpp \
    $$PWD/dialogs/usersharepasswordsettingdialog.cpp\
    $$PWD/gvfs/mountsecretdiskaskpassworddialog.cpp \
    $$PWD/views/drenamebar.cpp \
    $$PWD/shutil/filebatchprocess.cpp \
    $$PWD/dialogs/ddesktoprenamedialog.cpp \
    $$PWD/dialogs/dmultifilepropertydialog.cpp \
    $$PWD/dialogs/movetotrashconflictdialog.cpp \
    $$PWD/dialogs/dfmsettingdialog.cpp \
    $$PWD/controllers/operatorrevocation.cpp \
    $$PWD/controllers/tagcontroller.cpp \
    $$PWD/models/tagfileinfo.cpp \
    $$PWD/views/dtagactionwidget.cpp \
    $$PWD/views/droundbutton.cpp \
    $$PWD/views/dtagedit.cpp \
    $$PWD/shutil/dsqlitehandle.cpp \
    $$PWD/shutil/danythingmonitorfilter.cpp \
    $$PWD/controllers/tagmanagerdaemoncontroller.cpp \
    $$PWD/controllers/interface/tagmanagerdaemon_interface.cpp \
    $$PWD/interfaces/dfmsettings.cpp \
    $$PWD/interfaces/dfmcrumbbar.cpp \
    $$PWD/interfaces/dfmcrumbinterface.cpp \
    $$PWD/interfaces/plugins/dfmcrumbplugin.cpp \
    $$PWD/interfaces/plugins/dfmcrumbfactory.cpp \
    $$PWD/interfaces/dfmcrumbmanager.cpp \
    $$PWD/controllers/dfmfilecrumbcontroller.cpp \
    $$PWD/controllers/dfmcomputercrumbcontroller.cpp \
    $$PWD/controllers/dfmbookmarkcrumbcontroller.cpp \
    $$PWD/controllers/dfmtagcrumbcontroller.cpp \
    $$PWD/controllers/dfmnetworkcrumbcontroller.cpp \
    $$PWD/controllers/dfmtrashcrumbcontroller.cpp \
    $$PWD/controllers/dfmusersharecrumbcontroller.cpp \
    $$PWD/controllers/dfmavfscrumbcontroller.cpp \
    $$PWD/controllers/dfmsmbcrumbcontroller.cpp \
    $$PWD/controllers/dfmmtpcrumbcontroller.cpp \
    $$PWD/views/dfmaddressbar.cpp \
    $$PWD/views/dcompleterlistview.cpp \
    $$PWD/interfaces/dfmapplication.cpp \
    $$PWD/controllers/dfmsearchcrumbcontroller.cpp\
    $$PWD/views/dfmheaderview.cpp \
    $$PWD/controllers/mountcontroller.cpp \
    $$PWD/models/mountfileinfo.cpp \
    $$PWD/models/recentfileinfo.cpp \
    $$PWD/controllers/recentcontroller.cpp \
    $$PWD/controllers/dfmrecentcrumbcontroller.cpp \
    $$PWD/views/dfmadvancesearchbar.cpp \
    $$PWD/shutil/dfmregularexpression.cpp \
    $$PWD/models/mergeddesktopfileinfo.cpp \
    $$PWD/controllers/dfmmdcrumbcontrooler.cpp \
    $$PWD/controllers/mergeddesktopcontroller.cpp \
    $$PWD/controllers/masteredmediacontroller.cpp \
    $$PWD/controllers/dfmmasteredmediacrumbcontroller.cpp \
    $$PWD/models/masteredmediafileinfo.cpp \
    $$PWD/views/dfmopticalmediawidget.cpp \
    $$PWD/views/dfmrightdetailview.cpp \
    $$PWD/dialogs/burnoptdialog.cpp \
    $$PWD/interfaces/dfmcrumblistviewmodel.cpp \
    $$PWD/interfaces/dfmstyleditemdelegate.cpp \
    $$PWD/views/dfmsidebaritemdelegate.cpp \
    $$PWD/models/dfmsidebarmodel.cpp \
    $$PWD/views/dfmsidebarview.cpp \
    $$PWD/interfaces/dfmsidebariteminterface.cpp \
    $$PWD/interfaces/dfmsidebarmanager.cpp \
    $$PWD/controllers/dfmsidebardeviceitemhandler.cpp \
    $$PWD/controllers/dfmsidebartagitemhandler.cpp \
    $$PWD/controllers/dfmsidebardefaultitemhandler.cpp \
    $$PWD/controllers/dfmsidebarbookmarkitemhandler.cpp \
    $$PWD/interfaces/dfmsidebaritem.cpp \
    $$PWD/views/dfmsidebar.cpp \
    $$PWD/views/dfmfilebasicinfowidget.cpp \
    $$PWD/views/dfmtagwidget.cpp \
    $$PWD/controllers/vaultcontroller.cpp \
    $$PWD/models/vaultfileinfo.cpp \
    $$PWD/controllers/dfmvaultcrumbcontroller.cpp \
    $$PWD/views/dfmvaultunlockpages.cpp \
    $$PWD/views/dfmvaultfileview.cpp \
    $$PWD/views/dfmvaultrecoverykeypages.cpp \
    $$PWD/plugins/dfmadditionalmenu.cpp \
    $$PWD/dialogs/connecttoserverdialog.cpp \
    $$PWD/shutil/dfmfilelistfile.cpp \
    $$PWD/views/dfmsplitter.cpp \
    $$PWD/dbus/dbussysteminfo.cpp \
    $$PWD/models/deviceinfoparser.cpp \
    $$PWD/controllers/dfmsidebarvaultitemhandler.cpp \
    $$PWD/controllers/vaulthandle.cpp \
    $$PWD/views/dfmvaultremovepages.cpp \
    $$PWD/views/dfmvaultactiveview.cpp \
    $$PWD/views/dfmvaultactivestartview.cpp \
    $$PWD/views/dfmvaultactivesavekeyview.cpp \
    $$PWD/views/dfmvaultactivefinishedview.cpp \
    $$PWD/views/dfmvaultactivesetunlockmethodview.cpp \
    $$PWD/views/dfmvaultremoveprogressview.cpp \
    $$PWD/views/dfmvaultremovebypasswordview.cpp \
    $$PWD/views/dfmvaultremovebyrecoverykeyview.cpp \
    $$PWD/views/dfmvaultpagebase.cpp \
    $$PWD/bluetooth/bluetoothmanager.cpp \
    $$PWD/bluetooth/bluetoothadapter.cpp \
    $$PWD/bluetooth/bluetoothdevice.cpp \
    $$PWD/bluetooth/bluetoothmodel.cpp \
    $$PWD/bluetooth/bluetoothtransdialog.cpp \
    $$PWD/interfaces/drootfilemanager.cpp \
    $$PWD/interfaces/dgvfsfileinfo.cpp \
    $$PWD/shutil/checknetwork.cpp

!CONFIG(DISABLE_ANYTHING) {
    HEADERS += $$PWD/shutil/danythingmonitor.h
    SOURCES += $$PWD/shutil/danythingmonitor.cpp
}

INCLUDEPATH += $$PWD
