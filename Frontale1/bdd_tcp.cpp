#include "bdd_tcp.h"
#include <iostream>
#include <QTcpSocket>
#include <QTcpServer>
#include <openssl/evp.h>
#include <string>
#include <sstream>

using namespace std;

bdd_tcp::bdd_tcp()
{
}




void bdd_tcp::connection_tcp(QString IP, int port){

    soc.connectToHost(IP,port); 

}

void bdd_tcp::emission(string texte, int type){
        ostringstream oss;
    if(type == 0){
        texte +="*";
        texte = this->chiffrement(texte);
        oss << texte.size() ;
        oss << '*';
        oss << texte;
     }else {
        oss << texte;
    }



    QTextStream t(&soc); 
    t << QString(oss.str().data()) << endl; 

}

void bdd_tcp::attendLecture(int timeout, int type){

    
   
    ostringstream oss;

   yLecture=soc.waitForReadyRead(timeout);  

 

    while(soc.canReadLine()){

      oss << soc.readAll().data(); 
	}
      if(type == 0) msg = this->dechiffrement(oss.str());
      else msg = oss.str();

	if(msg == "")
		yLecture = false;	


}

void bdd_tcp::lecture(){
    ostringstream oss;
    while(soc.canReadLine()){
       oss << soc.readAll().data(); 
       msg=oss.str();
    }

}

string bdd_tcp::getMsg(){
    return this->msg;
}

bool bdd_tcp::getYLecture(){
    return yLecture;
}

string bdd_tcp::chiffrement(std::string clair){
    EVP_CIPHER_CTX ctx;
    int succ;
    int outlen, tmplen;
    ostringstream oss;
    
    char* out = (char*) malloc ( 2048 * sizeof ( char ) ) ;

    EVP_CIPHER_CTX_init ( &ctx ) ;

    succ = EVP_CipherInit_ex ( &ctx, EVP_aes_256_cbc(), NULL,(const unsigned char *) key,(const unsigned char *) iv, 1 );



    succ = EVP_CipherUpdate(&ctx,(unsigned char *)out  ,&outlen, (const unsigned char *)clair.c_str(), clair.length());


    succ = EVP_CipherFinal_ex(&ctx,(unsigned char *) out + outlen,&tmplen);



    succ = EVP_CIPHER_CTX_cleanup(&ctx);


    oss << out;

    free(out);

    return oss.str();



}

std::string bdd_tcp::dechiffrement(string chif){

    EVP_CIPHER_CTX ctx;
    int succ;
    int outlen, tmplen;


    char* out = (char*) malloc ( 2048 * sizeof ( char ) ) ;

    EVP_CIPHER_CTX_init ( &ctx );

    succ = EVP_CipherInit_ex ( &ctx, EVP_aes_256_cbc(), NULL,(const unsigned char *) key,(const unsigned char *) iv, 0 );



    EVP_CIPHER_CTX_set_padding ( &ctx, 0 ) ;

    succ = EVP_CipherUpdate(&ctx,(unsigned char *)out  ,&outlen, (const unsigned char *)chif.c_str(), chif.length());



    succ = EVP_CipherFinal_ex(&ctx,(unsigned char *) out + outlen,&tmplen);


    succ = EVP_CIPHER_CTX_cleanup(&ctx);


    std::string clair(out);

    free(out);

    return clair;
}

void bdd_tcp::setKeyIv(char *key, char *iv){
    this->key = key;
    this->iv = iv;
}
