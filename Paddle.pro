#-------------------------------------------------
#
# Project created by QtCreator 2016-07-15T13:54:17
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Paddle
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialworker.cpp

HEADERS  += mainwindow.h \
    serialworker.h

FORMS    += mainwindow.ui
