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
    controldialog.cpp \
    extend/controlpanel_extend_astar.cpp \
    extend/controlpanelextend.cpp \
    findpath/astar.cpp \
    findpath/bfs.cpp \
    findpath/bstar.cpp \
    findpath/dfs.cpp \
    findpath/findpath_base.cpp \
    main.cpp \
    mainwindow.cpp \
    world.cpp

HEADERS += \
    colorlabel.h \
    controldialog.h \
    extend/controlpanel_extend_astar.h \
    extend/controlpanelextend.h \
    findpath/astar.h \
    findpath/bfs.h \
    findpath/bstar.h \
    findpath/dfs.h \
    findpath/findpath_base.h \
    mainwindow.h \
    utils/utils.h \
    world.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
