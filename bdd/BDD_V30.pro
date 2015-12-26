TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    bdd_request.c \
    bdd.c \
    crypt.c \
    res.c

include(deployment.pri)
qtcAddDeployment()

DISTFILES += \
    READ_ME.txt \
    Makefile \
    bdd.sql

HEADERS += \
    bdd.h \
    crypt.h \
    default.h \
    res.h

