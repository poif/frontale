#include "rsacrypt.h"
#include <iostream>
#include <openssl/bio.h>
#include <openssl/pem.h>


using namespace std;

rsaCrypt::rsaCrypt(int keylen)
{
    this->keyLen = keylen;

    rsaKey = RSA_new();
    forgeignKey = RSA_new();

    cout << "fin constructeur" << endl;

}

QString rsaCrypt::chiffrement(QString clair){
    cout << "chiffrement rsa" << endl;
    unsigned char *chif = (unsigned char*)malloc(1024*sizeof(char));
    int succ;
    int kLen = RSA_size(forgeignKey) - 11;
    succ = RSA_public_encrypt(kLen,(const unsigned char *)clair.toStdString().c_str(),chif,forgeignKey,RSA_PKCS1_PADDING);
    if(succ <= 0)
            cout << "erreur RSA_encrypt" << endl;

    QString chiffre = QString((const char*)chif);

    return chiffre;





}

void rsaCrypt::keyGen()
{
    cout << "generation de clé rsa" << endl;
    BIGNUM *bne;
    bne = BN_new();
    int succ;

    if(bne == NULL)
        cout << "erreur BN_new" << endl;

    succ = BN_set_word(bne, 65537);
    if(succ == 0)
        cout << "erreur BN_set_word" << endl ;
    else
        cout << "succes"  << endl;

    succ=RSA_generate_key_ex(rsaKey,keyLen,bne ,NULL );
    if(succ != 1)
        cout << "erreur keygen" << endl;

    BN_free(bne);
}

QString rsaCrypt::dechiffrement(QString chif){
    cout << "dechiffrement" << endl;
    unsigned char *clair=(unsigned char*)malloc(1024*sizeof(char));;
    int succ;
    int kLen = RSA_size(rsaKey);

    succ = RSA_private_decrypt(kLen,(const unsigned char *)chif.toStdString().c_str(),clair,rsaKey,RSA_PKCS1_PADDING);

    if (succ <= 0)
        cout << "erreur dechiffrement rsa" << endl;

    return QString((const char*)clair);
}

QString rsaCrypt::clePub(){
    int flen = 0 ;
    unsigned char* key = NULL ;
    BIO *keybio = NULL ;

    int succ;


    flen = RSA_size ( rsaKey) + 170 ;
    key = (unsigned char*) malloc ( flen * sizeof ( unsigned char ) ) ;

    keybio = BIO_new ( BIO_s_mem() );

    if(keybio == NULL)
        cout << "erreur bio new" << endl;

     succ = PEM_write_bio_RSAPublicKey ( keybio, rsaKey );

     if (succ==0)
        cout << "erreur pem write" << endl;


     if ( BIO_read ( keybio, key, flen ) != flen )
        cout << "erreur bio read" << endl;

     return QString((const char *)key);
}
