#include "message.h"
#include <QCoreApplication>
#include <openssl/aes.h>
#include <cstdlib>
#include <ctime>
#include <string>
Message::Message(QString msg, char type, char separateur)
{
    this->msg = msg;
    this->type = type;
    this->separateur = separateur;

}

Message::Message(QString chiffre, char separateur){
    this->chiffre = chiffre;
    this->separateur = separateur;
}


QString Message::getMsg(){
    return this->msg;
}

QString Message::getChiffre(){
    return this->chiffre;
}

void Message::entete(){
    this->msg = QString("%1%2%3").arg(this->type).arg(this->separateur).arg(this->msg);
}

void Message::challenge(int N){
    srand(time(NULL));
    int chal;
    chal = N*(rand()/(double)RAND_MAX)*100;

    this->msg = QString("%1%2%3").arg(this->msg).arg(this->separateur).arg(chal);
}

void Message::chiffrement(const unsigned char *key){
    unsigned char trame[1024];
    const char *message = msg.toStdString().c_str();

    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    AES_KEY enc_key;
    AES_set_encrypt_key(key,128,&enc_key);
    AES_cbc_encrypt((const unsigned char *)message,trame,strlen(message), &enc_key,iv,AES_ENCRYPT );

    this->chiffre = QString((const char *)trame);

}

void Message::dechiffrement(const unsigned char *key){
    unsigned char trame [1024];
    const char *chif = this->chiffre.toStdString().c_str();

    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    AES_KEY dec_key;
    AES_set_decrypt_key(key,128,&dec_key);
    AES_cbc_encrypt((const unsigned char *)chif,trame,strlen(chif),&dec_key, iv,AES_DECRYPT);

    this->msg = QString((const char *)trame);
}
