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
    QString msg = this->chiffrement(texte.toStdString());
    QTextStream t(&soc); //création d'un flux d'ecriture dnas la socket
    t << texte << endl; // envoie du message en ecrivant dans le flux de la socket

}

void bdd_tcp::attendLecture(int timeout){

    QString chif;

   yLecture=soc.waitForReadyRead(timeout);  // attend que le paquet soit pret a etre lut

   QTextStream(stdout) << "it's ok" << endl;



      chif = soc.readLine(); //lecture du flux

      msg = QString(this->dechiffrement(chif));


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

QString bdd_tcp::chiffrement(string clair){
    EVP_CIPHER_CTX ctx;
    int succ;
    int outlen, tmplen;

    char* out = (char*) malloc ( 2048 * sizeof ( char ) ) ;

    EVP_CIPHER_CTX_init ( &ctx ) ;

    succ = EVP_CipherInit_ex ( &ctx, EVP_aes_256_cbc(), NULL,(const unsigned char *) key,(const unsigned char *) iv, 1 );

    if(!succ)
        cout << "erreur cipher init" << endl;

    succ = EVP_CipherUpdate(&ctx,(unsigned char *)out  ,&outlen, (const unsigned char *)clair.c_str(), clair.length());

    if(!succ)
        cout << "erreur aes cipher update" << endl;

    succ = EVP_CipherFinal_ex(&ctx,(unsigned char *) out + outlen,&tmplen);

    if(!succ)
        cout << "erreur aes cipher final" << endl;

    succ = EVP_CIPHER_CTX_cleanup(&ctx);

    if(!succ)
        cout << "erreur aes cleanup" << endl;

    free(out);

    return QString(out);

}

string bdd_tcp::dechiffrement(QString chif){

    EVP_CIPHER_CTX ctx;
    int succ;
    int outlen, tmplen;


    char* out = (char*) malloc ( 2048 * sizeof ( char ) ) ;

    EVP_CIPHER_CTX_init ( &ctx );

    succ = EVP_CipherInit_ex ( &ctx, EVP_aes_256_cbc(), NULL,(const unsigned char *) key,(const unsigned char *) iv, 0 );

    if(!succ)
        cout << "erreur cipher init" << endl;

    EVP_CIPHER_CTX_set_padding ( &ctx, 0 ) ;

    succ = EVP_CipherUpdate(&ctx,(unsigned char *)out  ,&outlen, (const unsigned char *)chif.toStdString().c_str(), chif.toStdString().length());

    if(!succ)
        cout << "erreur aes cipher update" << endl;

    succ = EVP_CipherFinal_ex(&ctx,(unsigned char *) out + outlen,&tmplen);

    if(!succ)
        cout << "erreur aes cipher final" << endl;

    succ = EVP_CIPHER_CTX_cleanup(&ctx);

    if(!succ)
        cout << "erreur aes cleanup" << endl;

    string clair(out);

    free(out);

    return clair;
}

void bdd_tcp::setKeyIv(char *key, char *iv){
    this->key = key;
    this->iv = iv;
}
