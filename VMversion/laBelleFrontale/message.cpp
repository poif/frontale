#include "message.h"
#include <QCoreApplication>
#include <openssl/aes.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <QTextStream>
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
    AES_set_encrypt_key(key,256,&enc_key);
    AES_cbc_encrypt((const unsigned char *)message,trame,strlen(message), &enc_key,iv,AES_ENCRYPT );

    this->chiffre = QString((const char *)trame);

}

std::string Message::crypt(unsigned char* aes_input, int size_aes_input){
    
    unsigned char trame [1024];
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    const static unsigned char aes_key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88};


    AES_KEY enc_key;
    AES_set_encrypt_key(aes_key, sizeof(aes_key)*8, &enc_key);
    AES_cbc_encrypt(aes_input, trame, size_aes_input, &enc_key, iv, AES_ENCRYPT);
    //print_data("\n Encrypted",enc_out, strlen(enc_out));

    std::string retour = (const char *) trame;
    return retour;

 }

std::string Message::decrypt(unsigned char* dec_in, int size_aes_input){

    unsigned char trame [1024];
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);
    const static unsigned char aes_key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88};

    AES_KEY dec_key;
    AES_set_decrypt_key(aes_key, sizeof(aes_key)*8, &dec_key);
    AES_cbc_encrypt(dec_in, trame, size_aes_input, &dec_key, iv, AES_DECRYPT);
    // print_data("\n Decrypted",dec_out, sizeof(dec_out));
    printf("%s\n", trame);

    std::string retour = (const char *) trame;
    return retour;
 }

/* fixit : probleme avec le dechiffrement depuis QT 5.5*/
void Message::dechiffrement(const unsigned char *key){
    unsigned char trame [1024];
    QTextStream(stdout) << this->chiffre ;
    std::string chif = this->chiffre.toStdString();

    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    std::cout << "ici crypt : " << chif <<  "taillt : " << chif.size() << std::endl;

    AES_KEY dec_key;
    AES_set_decrypt_key(key, sizeof(key)*8,&dec_key);
    AES_cbc_encrypt((const unsigned char *)chif.data(),trame,chif.size(),&dec_key, iv,AES_DECRYPT);
    std::cout << "ici crypt : " << chif <<  "taillt : " << chif.size() << std::endl;

    this->msg = QString((const char *)trame);
}
