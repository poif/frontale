#include "emission_tcp.h"

emission_tcp::emission_tcp()
{
}


void emission_tcp::connection_tcp(QString IP, int port){

    soc.connectToHost(IP,port); // bind de la socket tcp

}

void emission_tcp::emission(QString texte){
    QTextStream t(&soc); //création d'un flux d'ecriture dnas la socket
    t << texte << endl; // envoie du message en ecrivant dans le flux de la socket

}
