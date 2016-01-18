TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -L/usr/lib -lboost_system -lboost_serialization

SOURCES += main.cpp \
    noeudthor.cpp\
    trame.cpp

HEADERS += \
    client.h \
    noeudthor.h \
    trame.h

QMAKE_CXXFLAGS += -std=c++11
