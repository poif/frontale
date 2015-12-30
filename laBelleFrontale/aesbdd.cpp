#include "aesbdd.h"
#include <openssl/evp.h>
#include <iostream>

using namespace std;

aesBdd::aesBdd(char *key, char *iv)
{
    this->key=key;
    this->iv=iv;
}

QString aesBdd::chiffrement(string clair){
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

string aesBdd::dechiffrement(QString chif){

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
