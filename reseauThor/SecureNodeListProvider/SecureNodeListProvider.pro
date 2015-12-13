TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -L/usr/lib -lboost_system -lboost_serialization

SOURCES += main.cpp \
    securenodelistprovider.cpp \
    trame.cpp

HEADERS += \
    client.h \
    securenodelistprovider.h \
    trame.h

