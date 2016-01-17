#-------------------------------------------------
#
# Project created by QtCreator 2016-01-16T21:24:43
#
#-------------------------------------------------

QT       += core

QT       -= gui

QT       += network

TARGET = clientRSA
CONFIG   += console
CONFIG   -= app_bundle

LIBS += -lssl -lcrypto

TEMPLATE = app


SOURCES += main.cpp \
    client.cpp \
    reception.cpp \
    rsacrypt.cpp

HEADERS += \
    clientFront.h \
    reception.h \
    rsacrypt.h
