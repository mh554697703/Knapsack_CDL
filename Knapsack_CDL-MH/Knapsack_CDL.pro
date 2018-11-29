#-------------------------------------------------
#
# Project created by QtCreator 2017-09-02T10:19:25
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Knapsack_CDL
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
    admintoolbar.cpp \
    calculate\dswf.cpp \
    calculate\fswf.cpp \
    devices\adq214.cpp \
    devices\compass.cpp \
    devices\laserpulse.cpp \
    devices\laserseed.cpp \
    devices\motor.cpp \
    devicescontrol.cpp \
    display\rt_display.cpp \
    display\rt_scale.cpp \
    display\rt_title.cpp \
    display\stdata.cpp \
    display\stgraph.cpp \
    display\wind_display.cpp \
    display\linechart.cpp \
    display\barchart.cpp \
    display\polarchart.cpp \
    dispsettings.cpp \
    mainwindow.cpp \
    parametersetdialog.cpp \
    settingfile.cpp \
    usertoolbar.cpp \
    devices/serialportthread.cpp

HEADERS += \
        mainwindow.h \
    admintoolbar.h \
    calculate\dswf.h \
    calculate\fswf.h \
    devices\adq214.h \
    devices\compass.h \
    devices\laserpulse.h \
    devices\laserseed.h \
    devices\motor.h \
    devicescontrol.h \
    display\rt_display.h \
    display\rt_scale.h \
    display\rt_title.h \
    display\stdata.h \
    display\stgraph.h \
    display\wind_display.h \
    display\linechart.h \
    display\barchart.h \
    display\polarchart.h \
    dispsettings.h \
    global_defines.h \
    parametersetdialog.h \
    settingfile.h \
    usertoolbar.h \
    devices/serialportthread.h

FORMS += \
        mainwindow.ui \
    parametersetdialog.ui

win32: LIBS += -L$$quote(C:/Program Files/SP Devices/ADQAPI/) -lADQAPI
LIBS += -LC:/nlopt -lnlopt

INCLUDEPATH += \
    $$quote(C:/Program Files/SP Devices/ADQAPI/)\
    C:/eigen/\
    C:/nlopt/

RESOURCES += \
    icons.qrc

CONFIG += qwt
