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
    soc->bind(12345,QUdpSocket::ShareAddress);
    QTextStream(stdout) << "fin bind" << endl;

    connect(soc,SIGNAL(readyRead()),this, SLOT(procReception()));

    QTextStream(stdout) << "connexion" << endl;

}

void reception::procReception(){

        QByteArray datagram;
        datagram.resize(soc->pendingDatagramSize());
        soc->readDatagram(datagram.data(),datagram.size());
	
        const unsigned char key[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}; 

        QString message;

        message = QString(datagram.data());
	

        Message msg(message,'*');
        Requete req;

        string * showRep = new string[2];

	QTextStream(stdout) << datagram.data() << endl;	        

        msg.dechiffrement(key);

	QTextStream(stdout) << msg.getMsg() << endl;

        if(req.decoupage(msg.getMsg().toStdString().c_str())){
        req.construction();
        string temp = req.getRequete();

        /* Il faudrait plutôt utiliser une réference vers l'objet de type network_interface qui doit rester unique */
        network_interface netinf;
        cout << temp << endl;
        showRep = netinf.send_look(temp);

        string sep = "*";

        char * triq = new char(showRep[0].size()+showRep[1].size()+sep.size()+1);
        triq = const_cast <char *>((showRep[0] + sep.data() + showRep[1]).data());
        req.tri(triq);

        QString retour;
        retour = QString("%1").arg(req.getResultat());

        Message msg2(retour,'R', '*');
        msg2.entete();
        msg2.chiffrement(key);
        client cli;
        cli.socBind();
        cli.emission(msg2.getChiffre());}

}
