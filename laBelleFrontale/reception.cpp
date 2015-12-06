#include "reception.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>

reception::reception()
{
}


void reception::ecoute(){
    soc = new QUdpSocket(this);
    soc->bind(12345,QUdpSocket::ShareAddress);
    soc->waitForReadyRead(-1);
    procReception();

}

void reception::procReception(){
    QByteArray datagram;
    datagram.resize(soc->pendingDatagramSize());
    soc->readDatagram(datagram.data(),datagram.size());
    msg=QString("%1").arg(datagram.data());


}
QString reception::getMsg(){
    return this->msg;
}
