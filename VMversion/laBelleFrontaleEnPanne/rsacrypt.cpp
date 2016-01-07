#include "rsacrypt.h"
#include <iostream>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <string>
#include <sstream>
#include <QTextStream>

using namespace std;

rsaCrypt::rsaCrypt(int keylen)
{
    this->keyLen = keylen;

    rsaKey = RSA_new();
    forgeignKey = RSA_new();

    cout << "fin constructeur" << endl;

}

string rsaCrypt::chiffrement(string clair){
    cout << "chiffrement rsa" << endl;
    ostringstream oss;
    unsigned char *chif = (unsigned char*)malloc(1024*sizeof(char));
    int succ;
    int kLen = RSA_size(forgeignKey) - 11;
    succ = RSA_public_encrypt(kLen,(const unsigned char *)clair.c_str(),chif,forgeignKey,RSA_PKCS1_PADDING);
    if(succ <= 0)
            cout << "erreur RSA_encrypt" << endl;

    oss << chif;

    return oss.str();





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

string rsaCrypt::dechiffrement(string chif){
    cout << "dechiffrement" << endl;
    ostringstream oss;
    unsigned char *clair=(unsigned char*)malloc(1024*sizeof(char));;
    int succ;
    int kLen = RSA_size(rsaKey);


    succ = RSA_private_decrypt(kLen,(const unsigned char *)chif.c_str(),clair,rsaKey,RSA_PKCS1_PADDING);

    if (succ < 0)
        cout << "erreur dechiffrement rsa" << endl;

    oss << chif;

    return oss.str();
}

string rsaCrypt::sendKeyPub(int id){
    int flen = 0 ;
    unsigned char* key = NULL ;
    BIO *keybio = NULL ;
    ostringstream msg;
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

     cout << key << endl;

     msg << id + "*" + "304*" + key + "*EOF";

     return msg.str();
}

bool rsaCrypt::recupKeyPub(string key){
    istringstream iss;
    string bddKey;
    ostringstream oss;
    BIO *keybio = NULL ;
   /* oss << id;
    iss << key;

    getline(iss,bddKey,'*');

    if(!bddKey.compare(oss.str())){
        getline(iss,bddKey,'*');

        if(!bddKey.compare("init")){
            getline(iss,bddKey,'*');
            cout << bddKey << endl;*/

            keybio = BIO_new_mem_buf ((unsigned char *)bddKey.c_str() , -1 );

            forgeignKey=PEM_read_bio_RSAPublicKey ( keybio, &forgeignKey, NULL, NULL );

           /* return true;
        }else {
           return false;
           }
    }else {
            return false;
        }*/


}

bool rsaCrypt::recupAesKey(string key, int id){

    istringstream iss;
    string bddKey;
    ostringstream oss;

    oss << id;
    iss.str(key);

    getline(iss,bddKey,'*');

    if(!bddKey.compare(oss.str())){
        getline(iss,bddKey,'*');

        if(!bddKey.compare("304")){
            getline(iss,bddKey,'*');
            cout << bddKey << endl;

            istringstream issAes;
            string bddaAes;

            issAes.str(this->dechiffrement(QString(bddKey.c_str())).toStdString());

            getline(issAes,bddaAes,';');
            aesKey = (char *)bddaAes.c_str();

            getline(issAes,bddaAes,'*');
            aesIv = (char *)bddaAes.c_str();

            return true;
        }else {
            return false;
        }
    }else {
        return false;
    }
}

char * rsaCrypt::getAesKey(){
    return aesKey;
}

char * rsaCrypt::getAesIv(){
    return aesIv;
}