#include "bdd_tcp.h"
#include <iostream>
#include <QTcpSocket>
#include <QTcpServer>
bdd_tcp::bdd_tcp()
{
}




void bdd_tcp::connection_tcp(QString IP, int port){

    soc.connectToHost(IP,port); // bind de la socket tcp

}

void bdd_tcp::emission(QString texte){
    QTextStream t(&soc); //création d'un flux d'ecriture dnas la socket
    t << texte << endl; // envoie du message en ecrivant dans le flux de la socket

}

void bdd_tcp::attendLecture(){

   yLecture=soc.waitForReadyRead(-1);  // attend que le paquet soit pret a etre lut

   QTextStream(stdout) << "it's ok" << endl;

//   while(soc.canReadLine()){

      msg = soc.readLine(); //lecture du flux



  // }

}

void bdd_tcp::lecture(){
    while(soc.canReadLine()){
       msg = soc.readLine(); //lecture du flux

    }

}

QString bdd_tcp::getMsg(){
    return this->msg;
}

bool bdd_tcp::getYLecture(){
    return yLecture;
}
