#include "clientFront.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <QTextStream>

using namespace std;

clientFront::client()
{
}

void clientFront::socBind(){
    soc = new QUdpSocket(this);

}

void clientFront::emission(QString message){

    QByteArray datagram = message.toAscii().data();
    soc->writeDatagram(datagram.data(),datagram.size(), QHostAddress::LocalHost, 1234 );

}


