#include "reception.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>

reception::reception()
{
}


void reception::ecoute(int timeout){
    soc = new QUdpSocket(this);
    soc->bind(12346,QUdpSocket::ShareAddress);
    soc->waitForReadyRead(timeout);
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
