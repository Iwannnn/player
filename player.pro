QT       += core gui widgets

TEMPLATE = app
CONFIG   += c++17 console
TARGET = player_demo

INCLUDEPATH += include
DEPENDPATH  += include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mainwindow.ui

HEADERS += \
    include/mainwindow.h

