#include "reception_tcp.h"
#include <QTcpSocket>

reception_tcp::reception_tcp()
{
}


void reception_tcp::bind(){


    listen(QHostAddress::Any,32154); //ecoute sur le port 32154

    connexion = waitForNewConnection(10000); // attend 10000ms que la connexion s'etablisse

}


void reception_tcp::attenteLecture(){

   soc=nextPendingConnection();

   soc->waitForReadyRead(-1);  // attend que le paquet soit pret a etre lut

   lecture();

}

void reception_tcp::lecture(){
    while(soc->canReadLine()){
       msg = soc->readLine(); //lecture du flux
    }

}

QString reception_tcp::getMsg(){
    return this->msg;
}

bool reception_tcp::getConnexion(){
    return this->connexion;
}
