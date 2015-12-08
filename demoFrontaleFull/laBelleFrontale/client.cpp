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

    cout << message.toStdString() << endl;
    QByteArray datagram = message.toAscii().data();
    cout << datagram.size() << endl;
    soc->writeDatagram(datagram.data(),datagram.size(), QHostAddress::LocalHost, 1234 );

}


