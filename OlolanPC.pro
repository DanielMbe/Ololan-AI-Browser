QT       += core gui gui-private
greaterThan(QT_MAJOR_VERSION, 5): QT += widgets webenginewidgets sql
CONFIG += c++20
LIBS += gdi32.lib dwmapi.lib UxTheme.lib -luser32 -lwindowsapp -lShcore
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
    customtoolbar.cpp \
    customtoolbutton.cpp \
    customwindow.cpp \
    main.cpp \
    oappcore.cpp \
    oassistantcore.cpp \
    obookmarkscore.cpp \
    obookmarkwidget.cpp \
    obrowsermenu.cpp \
    ocertificatemanager.cpp \
    odownloadscore.cpp \
    odownloadwidget.cpp \
    ofindinput.cpp \
    ohistorycore.cpp \
    oinputcompleter.cpp \
    osettingscore.cpp \
    osmartinput.cpp \
    ostartpagecore.cpp \
    osyncwidget.cpp \
    otabbar.cpp \
    owebinspector.cpp \
    owebpage.cpp \
    owebsiteinfo.cpp \
    owebview.cpp \
    otabpage.cpp \
    owebwidget.cpp

HEADERS += \
    customtoolbar.h \
    customtoolbutton.h \
    customwindow.h \
    oappcore.h \
    oassistantcore.h \
    obookmarkscore.h \
    obookmarkwidget.h \
    obrowsermenu.h \
    ocertificatemanager.h \
    odownloadscore.h \
    odownloadwidget.h \
    ofindinput.h \
    ohistorycore.h \
    oinputcompleter.h \
    olibrary.h \
    osettingscore.h \
    osmartinput.h \
    ostartpagecore.h \
    osyncwidget.h \
    otabbar.h \
    owebinspector.h \
    owebpage.h \
    owebsiteinfo.h \
    owebview.h \
    otabpage.h \
    owebwidget.h
# Default rules for deployment.
TARGET = ololan
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ololanResource.qrc

RC_FILE = ololanAppLogo.rc
