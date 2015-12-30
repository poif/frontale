#include "reception.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <sstream>

reception::reception()
{
}


void reception::ecoute(int timeout){
    soc = new QUdpSocket(this);
    soc->bind(12345,QUdpSocket::ShareAddress);
    QObject::connect(&soc,SIGNAL(readyRead()), this,SLOT(procReception());

}

void reception::procReception(){
    QByteArray datagram;
    ostringstream oss;
    datagram.resize(soc->pendingDatagramSize());
    soc->readDatagram(datagram.data(),datagram.size(), &hostAddr, &hostPort);
    oss << datagram.data;
    fileMsg.push_back(oss.str());

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

list<string> reception::getFileMsg(){
    return fileMsg.pop_front();

}
