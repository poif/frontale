#include "reception_tcp.h"
#include <QTcpSocket>

reception_tcp::reception_tcp()
{
}


void reception_tcp::bind(){
    listen(QHostAddress::Any,32154); //ecoute sur le port 32154
    QObject::connect(this,SIGNAL(newConnection()),this,SLOT(lecture())); // appel de la methode lecture quand une nouvelle connexion est detectée

}


void reception_tcp::lecture(){
    soc=nextPendingConnection();

    QObject::connect(soc,SIGNAL(readyRead()),this,SLOT(affichage())); // appel de affichage lorsqu'un message est pret a etre lut

}

void reception_tcp::affichage(){
    QString msg;
    while(soc->canReadLine()){
        msg = soc->readLine();
        QTextStream(stdout) << msg << endl; //affichage du message sur stdout
    }
}
