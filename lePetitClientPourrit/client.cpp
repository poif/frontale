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

void clientFront::emission(string message,QHostAddress hostaddr, quint16 hostPort){

    QByteArray datagram = message.data();

    soc->writeDatagram(datagram, hostaddr, hostPort);

}

void clientFront::emission(string message){

    QHostAddress addr = QHostAddress::LocalHost;

    QByteArray datagram = message.data();

    soc->writeDatagram(datagram, addr,12345);

}

void clientFront::emission(char * message, int size){

    QHostAddress addr = QHostAddress::LocalHost;

    QByteArray datagram(message, size);

    soc->writeDatagram(datagram, addr,1234);

}
