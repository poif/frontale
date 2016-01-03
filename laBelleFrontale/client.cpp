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

    QByteArray datagram = message.toLatin1().data();

    soc->writeDatagram(datagram, hostaddr, hostPort);

}

void clientFront::emission(QString message){

    QHostAddress addr = QHostAddress::LocalHost;

    QByteArray datagram = message.toLatin1().data();

    soc->writeDatagram(datagram, addr,1234);

}