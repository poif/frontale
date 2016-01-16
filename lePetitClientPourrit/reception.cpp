#include "reception.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <sstream>
#include <stdio.h>

#include <openssl/aes.h>



reception::reception()
{
}


void reception::ecoute(){
    soc = new QUdpSocket(this);
    soc->bind(1234,QUdpSocket::ShareAddress);
    soc->waitForReadyRead(-1);//QObject::connect(soc,SIGNAL(readyRead()), this,SLOT(procReception()));
    procReception();
}




void reception::procReception(){
    QByteArray datagram;
    ostringstream oss;
    QHostAddress hostAddr;
    quint16 hostPort;
    datagram.resize(soc->pendingDatagramSize());
    soc->readDatagram(datagram.data(),datagram.size(), &hostAddr, &hostPort);
    oss << datagram.data();
    string jambon;

    istringstream iss;

    iss.str(oss.str());
    getline(iss,jambon,'*' );

    cout << jambon << endl;
    cout << "chiffre:   ";
    ostringstream yoloss;
    getline(iss,jambon,'*');
    yoloss << jambon;
    while(getline(iss,jambon,'*')){
        yoloss << "*";
        yoloss << jambon;

    }

    chiffre = yoloss.str();
    cout << chiffre << endl;
//    boost::thread my_thread(&reception::traitement, boost::ref(*this), oss.str());

}


string reception::getMsg(){
    return this->chiffre;
}

