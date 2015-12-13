#include "clientFront.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <QTextStream>

using namespace std;

clientFront::clientFront()
{
}

void clientFront::socBind(){
    soc = new QUdpSocket(this);

}

void clientFront::emission(QString message){

    QHostAddress addr = QHostAddress::LocalHost;
    QByteArray datagram = message.toAscii().data();

    soc->writeDatagram(datagram, addr, 1234);

}


