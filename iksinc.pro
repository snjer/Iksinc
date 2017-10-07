#-------------------------------------------------
#
# Project created by QtCreator 2017-09-26T21:25:52
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = iksinc
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    udpthread.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    udpthread.h

FORMS    += mainwindow.ui
