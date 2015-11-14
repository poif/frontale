#include "client.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <QTextStream>


client::client()
{
}

void client::socBind(){
    soc = new QUdpSocket(this);

}

void client::emission(const char *message){

    QByteArray datagram = message;
    soc->writeDatagram(datagram.data(),datagram.size(),QHostAddress::Broadcast, 45454 );

}


