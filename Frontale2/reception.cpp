#include "reception.h"
#include <QtNetwork>
#include <iostream>
#include <QCoreApplication>
#include <sstream>
#include <stdio.h>

#include <openssl/aes.h>
#include <boost/thread.hpp>


reception::reception(Tslot * ts, bdd_tcp * bdd,Message *ourMsg, network_interface * netinf): m_ts(ts), m_bdd(bdd), m_netinf(netinf), mess(ourMsg)
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
    cout << datagram.size() << endl;

    boost::thread my_thread(&reception::traitement, boost::ref(*this), oss.str());

}

void reception::procReception_s(){
    QByteArray datagram;
    ostringstream oss;
    datagram.resize(soc->pendingDatagramSize());
    soc->readDatagram(datagram.data(),datagram.size());
    oss << datagram.data();
    //TODO: verification mutex 
    //fileMsg.push_back(oss.str());

    cout << "icibas" << endl;
    cout << oss.str() << endl;

    string token = m_ts->getLastToken();
    cout << token << endl;
    boost::mutex * mustStopMutex = m_ts->getMutex(token);
    mustStopMutex->lock();
    bool mustStop = m_ts->getBool(token);
    if (mustStop == false)
        m_ts->addMessageToList(token, oss.str());
    mustStopMutex->unlock();

}

void reception::traitement(string messageStr){

        QHostAddress hoteCourant;
        quint16 portCourant;

        int numKey;

        //const static unsigned char key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88};
        
        hoteCourant = getHostAddr();
        portCourant = getHostPort();

        cout << "Génération du token..." << endl;
        string token = m_ts->GenToken(hoteCourant,portCourant);
        cout << "Token généré." << endl;

        cout << "Got somethin" << endl;

        Requete req;

        string input = mess->decrypt((unsigned char*) messageStr.data(), messageStr.size());
        
        if(mess->getEChangeKey()){

            clientFront cli;
            cli.socBind();
            cli.emission(mess->getToSendArray(), sizeof(char)*258);



        }else {

        numKey = mess->getNumClient();

        input = token + "*" + input;

        cout << "input : " << input << endl;

        if(req.decoupage(input)){
            cout << "tata" << endl;
            req.construction();
            cout << "tata" << endl;

            if(req.getPourBdd()){

                m_bdd->emission(req.getRequete());
                
                m_bdd->attendLecture(300);

                string messBdd = m_bdd->getMsg();

                list<string> listReponseBdd;
                listReponseBdd.push_back(messBdd);

                    req.tri(listReponseBdd);                            
            }
            else
            {

                string traitement = req.getRequete();

                string option = req.getOption();

                if(option.compare("-n")==0){/* traitement est une affectation */
                    m_netinf->send_look(traitement, token);
                    cout << "ici envoi" << endl;
                }
                else if(option.compare("-e")==0){/* traitement est un statut */
                    m_netinf->send_exist(traitement, token);
                }
                if(option.compare("-r")==0){
                    m_netinf->send_lookrec(option,traitement, token);
                }

                list<string> * listReponse = m_ts->startTimer(token, 5000);

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

            //msg2.entete();
            //msg2.chiffrement(key);

            cout << "resultat : " << req.getResultat() << endl;

            string repo = "1*R*" + req.getResultat();

            string chiffrer = mess->crypt((unsigned char*) repo.data(), repo.size(), numKey);

            //string toto = msg2.getMsg().toStdString();
            cout << chiffrer << endl;

            clientFront cli;
            cli.socBind();
            cli.emission(chiffrer);//, hoteCourant, portCourant);

        }
        }
}

QString reception::getMsg(){
    return this->m_msg;
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
