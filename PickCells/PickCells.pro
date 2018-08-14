#-------------------------------------------------
#
# Project created by QtCreator 2018-02-07T10:43:48
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PickCells
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    controller/cellsstates.cpp \
    controller/cell.cpp \
    view/cellsview.cpp \
    view/brick.cpp \
    view/appsview.cpp \
    controller/appscreen.cpp \
    view/widgets/button.cpp \
    view/scenes/testscene.cpp \
    view/scenes/colorscene.cpp \
    view/widgets/swatch.cpp \
    view/scenes/socialscene.cpp \
    view/scenes/lockscene.cpp \
    view/scenes/artscene.cpp \
    view/scenes/checkscene.cpp

HEADERS += \
        mainwindow.h \
    controller/cellsstates.h \
    controller/cell.h \
    view/cellsview.h \
    view/brick.h \
    view/appsview.h \
    controller/appscreen.h \
    view/widgets/button.h \
    view/scenes/scene.h \
    view/scenes/testscene.h \
    view/scenes/colorscene.h \
    view/widgets/swatch.h \
    view/scenes/socialscene.h \
    view/scenes/lockscene.h \
    view/scenes/artscene.h \
    view/scenes/checkscene.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc
