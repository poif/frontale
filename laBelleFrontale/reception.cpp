#include "reception.h"
#include <QCoreApplication>
#include <iostream>
#include <QTextStream>
#include "message.h"
#include "requete.h"
#include "network_interface.h"
#include <string>
#include "client.h"
using namespace std;


#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>

reception::reception()
{
}


void reception::ecoute(){
    soc = new QUdpSocket(this);
    soc->bind(45454,QUdpSocket::ShareAddress);
    QTextStream(stdout) << "fin bind" << endl;

    connect(soc,SIGNAL(readyRead()),this, SLOT(procReception()));

    QTextStream(stdout) << "connexion" << endl;

}

void reception::procReception(){

        QByteArray datagram;
        datagram.resize(soc->pendingDatagramSize());
        soc->readDatagram(datagram.data(),datagram.size());

        QString message;

        message = QString(datagram.data());


        Message msg(message,'*');
        Requete req;

        string * showRep = new string[2];
        char * triq = new char(showRep[0].size()+showRep[1].size()+1);

        msg.dechiffrement();
        req.decoupage(msg.getMsg().toStdString().c_str());
        req.construction();
        network_interface netinf;
        showRep = netinf.send_look(req.getRequete());


        triq = const_cast <char *>(showRep[0] + showRep[1]);
        req.tri(triq);



        QString retour;
        retour = QString("%1").arg(req.getResultat());

        Message msg2(retour,'R', '*');
        msg2.entete();
        msg2.chiffrement();
        client cli;
        cli.socBind();
        cli.emission(msg2.chiffre);





}
