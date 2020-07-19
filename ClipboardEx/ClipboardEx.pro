QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ClipboardTipsWindow.cpp \
    ConfigManager.cpp \
	screenshot/canvas/snipcanvaswidget.cpp \
    screenshot/snip/magnifierwidget.cpp \
    screenshot/snip/sniptoolbar.cpp \
    util/DockableWindow.cpp \
    HistoryDataList.cpp \
    MainControl.cpp \
    MenuActionWidget.cpp \
    screenshot/snip/snipwidget.cpp \
    main.cpp \
    mimedatalabel.cpp \
    util.cpp \
    util/floatlayout.cpp

HEADERS += \
    ClipboardTipsWindow.h \
    ConfigManager.h \
	screenshot/canvas/snipcanvaswidget.h \
    screenshot/snip/magnifierwidget.h \
    screenshot/snip/sniptoolbar.h \
    util/DockableWindow.h \
    HistoryDataList.h \
    MainControl.h \
    MenuActionWidget.h \
    screenshot/snip/snipwidget.h \
    def.h \
    mimedatalabel.h \
    util.h \
    util/floatlayout.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

TRANSLATIONS += res\i18n\zh_cn.ts \
	res\i18n\en_us.ts

include(./3rd_party/QHotkey/qhotkey.pri)
