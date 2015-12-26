#include "reception.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>

reception::reception()
{
}


void reception::ecoute(int timeout){
    soc = new QUdpSocket(this);
    soc->bind(12345,QUdpSocket::ShareAddress);
    expiration=soc->waitForReadyRead(timeout);
    procReception();

}

void reception::procReception(){
    QByteArray datagram;
    datagram.resize(soc->pendingDatagramSize());
    soc->readDatagram(datagram.data(),datagram.size(), hostAddr, hostPort);
    msg=QString("%1").arg(datagram.data());


}
QString reception::getMsg(){
    return this->msg;
}

bool reception::getExpiration(){
	return expiration;
}

QHostAddress reception::getHostAddr(){
        return this->hostAddr;
}

quint16 reception::getHostPort(){
       return this->hostPort;
}
