TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QMAKE_CXXFLAGS += -std=c++11
LIBS += -L/usr/lib -lboost_system -lboost_serialization -lcrypto -lcryptopp

# Input
HEADERS += bdd_tcp.h \
           client.h \
           clientFront.h \
           engine_event.h \
           message.h \
           network_interface.h \
           noeudthor.h \
           reception.h \
           requete.h \
           traitement.h \
           trame.h \
           utils.h \
    fusionneur.h \
    timertoken.h
SOURCES += bdd_tcp.cpp \
           client.cpp \
           main.cpp \
           message.cpp \
           network_interface.cpp \
           noeudthor.cpp \
           reception.cpp \
           requete.cpp \
           traitement.cpp \
           trame.cpp \
           utils.cpp \
    fusionneur.cpp \
    timertoken.cpp
QT += network
