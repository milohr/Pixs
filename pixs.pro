#-------------------------------------------------
#
# Project created by QtCreator 2017-06-11T21:08:38
#
#-------------------------------------------------

QT       += core gui
QT       += sql

qtHaveModule(printsupport): QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pixs
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
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
    db/dbactions.cpp \
    settings.cpp \
    db/dbthread.cpp \
    albumsview.cpp \
    album.cpp \
    scrolltext.cpp \
    collectionview.cpp

HEADERS += \
        mainwindow.h \
    pixsutils.h \
    utils.h \
    db/dbactions.h \
    settings.h \
    db/dbthread.h \
    albumsview.h \
    album.h \
    scrolltext.h \
    collectionview.h

FORMS += \
        mainwindow.ui \
    settings.ui

RESOURCES += \
    resources.qrc
