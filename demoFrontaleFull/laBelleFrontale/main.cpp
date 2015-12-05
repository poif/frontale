#include <QCoreApplication>
#include <message.h>
#include <requete.h>
#include "engine_event.h"
#include "network_interface.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <reception.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    //thread
    reception ser;
    ser.ecoute();


    //thread

    network_interface net;



    return a.exec();
}
