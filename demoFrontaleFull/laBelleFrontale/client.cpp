#include "client.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <QTextStream>

using namespace std;

client::client()
{
}

void client::socBind(){
    soc = new QUdpSocket(this);

}

void client::emission(QString message){

    QByteArray datagram = message.toAscii().data();;
    soc->writeDatagram(datagram.data(),datagram.size(),QHostAddress::Broadcast, 45454 );

}


