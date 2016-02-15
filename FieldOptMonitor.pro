#-------------------------------------------------
#
# Project created by QtCreator 2016-02-11T16:21:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FieldOptMonitor
TEMPLATE = app

CONFIG += c++11
CONFIG += qwt

SOURCES += main.cpp\
        mainwindow.cpp \
    logreader.cpp

HEADERS  += mainwindow.h \
    logreader.h

FORMS    += mainwindow.ui
