#include "reception.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <sstream>

reception::reception(Tslot * ts, network_interface * netinf): m_ts(ts), m_netinf(netinf)
{
}


void reception::ecoute(){
    soc = new QUdpSocket(this);
    soc->bind(12345,QUdpSocket::ShareAddress);
    QObject::connect(soc,SIGNAL(readyRead()), this,SLOT(procReception()));

}

void reception::ecoute_s(){
    soc = new QUdpSocket(this);
    soc->bind(54321,QUdpSocket::ShareAddress);
    QObject::connect(soc,SIGNAL(readyRead()), this,SLOT(procReception_s()));

}

void reception::procReception(){
    QByteArray datagram;
    ostringstream oss;
    datagram.resize(soc->pendingDatagramSize());
    soc->readDatagram(datagram.data(),datagram.size(), &hostAddr, &hostPort);
    oss << datagram.data();

    traitement(oss.str());

}

void reception::procReception_s(){
    QByteArray datagram;
    ostringstream oss;
    datagram.resize(soc->pendingDatagramSize());
    soc->readDatagram(datagram.data(),datagram.size());
    oss << datagram.data();
    //TODO: verification mutex 
    fileMsg.push_back(oss.str());

}

void traitement(string messageStr){

        QHostAddress hoteCourant;
        quint16 portCourant;

        bool respBool = false;
        QString message;
        QString versBdd;

        const unsigned char key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88};
        
        hoteCourant = ser.getHostAddr();
        portCourant = ser.getHostPort();

        cout << "Génération du token..." << endl;
        token = ts.GenToken(hoteCourant,portCourant);
        cout << "Token généré." << endl;

        message = QString("%1").arg(messageStr.data());

        cout << "Got somethin" << endl;
        Message msg(message,'*');
        Requete req;

        //string * showRep = new string[2];
        string showRep;

        //QTextStream(stdout) << ser.getFileMsg() << endl;          

        msg.dechiffrement(key);

        //QTextStream(stdout) << msg.getMsg() << endl;

        string input = msg.getMsg().toStdString();

        input = token + "*" + input;

        if(req.decoupage(input)){
            req.construction();

            if(req.getPourBdd()){
                versBdd = QString("%1").arg(req.getRequete().c_str());

                bdd.emission(versBdd);
                
                bdd.attendLecture();

                string messBdd = bdd.getMsg().toStdString();

                list<string> listReponseBdd;
                listReponseBdd.push_back(messBdd);

                    req.tri(listReponseBdd);                            
            }
            else
            {
                string traitement = req.getRequete();

                string option = req.getOption();

                if(option.compare("-n")==0){/* traitement est une affectation */
                    netinf.send_look(traitement, token);
                    cout << "ici envoi" << endl;
                }
                else if(option.compare("-e")==0){/* traitement est un statut */
                    netinf.send_exist(traitement, token);
                }
                if(option.compare("-p")==0){
                    string type = "picture";
                    netinf.send_lookrec(type,traitement, token);
                }

                list<string> listReponse = m_ts->startTimer(token, 500);

                if(!listReponse->empty()){

                req.tri(*listReponse);

                //TODO : methode qui delete les liés au token

                }
                else{
                    cout << "Temps maximum écoulé, pas de réponse" << endl;
                    req.setResultat("empty");
                }

            }

            QString retour;
            retour = QString("%1").arg(req.getResultat().c_str());

            Message msg2(retour,'R', '*');
            msg2.entete();
            msg2.chiffrement(key);

            string toto = msg2.getMsg().toStdString();
            cout << toto << endl;

            clientFront cli;
            cli.socBind();
            cli.emission(msg2.getChiffre(), hoteCourant, portCourant);

        }
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

string reception::getFileMsg(){
    return fileMsg.front();

    fileMsg.pop_front();

}
