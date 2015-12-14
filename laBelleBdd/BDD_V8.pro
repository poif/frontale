TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    bdd.c \
    bdd_request.c \
    crypt.c \
    res.c

DISTFILES += \
    READ_ME.txt \
    Makefile \
    bdd.sql

HEADERS += \
    bdd.h \
    crypt.h \
    default.h \
    res.h

