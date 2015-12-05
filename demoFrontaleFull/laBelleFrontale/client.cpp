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

    cout << message.toStdString() << endl;
    QByteArray datagram = message.toAscii().data();
    cout.write(datagram.data(),datagram.size());
    soc->writeDatagram(datagram.data(),datagram.size(), QHostAddress::LocalHost, 1234 );

}


