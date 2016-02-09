#-------------------------------------------------
#
# Project created by QtCreator 2016-02-07T12:46:33
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = paddle
TEMPLATE = app


SOURCES += main.cpp\
        mainworker.cpp \
    serialworker.cpp

HEADERS  += mainworker.h \
    serialworker.h

FORMS    += mainworker.ui
