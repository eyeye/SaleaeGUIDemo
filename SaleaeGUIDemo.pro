#-------------------------------------------------
#
# Project created by QtCreator 2013-06-01T23:03:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SaleaeGUIDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui





unix|win32: LIBS += -L$$PWD/SaleaeDeviceSdk-1.1.14/lib/ -lSaleaeDevice

INCLUDEPATH += $$PWD/SaleaeDeviceSdk-1.1.14/include
DEPENDPATH += $$PWD/SaleaeDeviceSdk-1.1.14/include


