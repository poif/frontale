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

void clientFront::emission(QString message,QHostAddress hostaddr, quint16 hostPort){

    QByteArray datagram = message.toAscii().data();

    soc->writeDatagram(datagram, hostaddr, hostPort);

}


