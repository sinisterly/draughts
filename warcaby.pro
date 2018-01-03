#-------------------------------------------------
#
# Project created by QtCreator 2017-10-17T18:49:15
#
#-------------------------------------------------

QT       += core gui
QT	 += network
CONFIG += C++14
CONFIG += warn_on

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = warcaby
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
    game.cpp \
    square.cpp \
    piece.cpp \
    man.cpp \
    king.cpp \
    emptypiece.cpp \
    connection.cpp

HEADERS += \
        mainwindow.h \
    game.h \
    square.h \
    enum.h \
    piece.h \
    man.h \
    king.h \
    emptypiece.h \
    connection.h

FORMS += \
        mainwindow.ui \
    connection.ui


