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
/*
string rsaCrypt::chiffrement(string clair){
    cout << "chiffrement rsa" << endl;
    cout << "laforeignkey" << forgeignKey << endl;
    ostringstream oss;
    unsigned char *chif = (unsigned char*)malloc(1024*sizeof(char));
    int succ;
    int kLen = RSA_size(forgeignKey) - 11;
    succ = RSA_public_encrypt(kLen,(const unsigned char *)clair.c_str(),chif,forgeignKey,RSA_PKCS1_PADDING);
    if(succ <= 0)
            cout << "erreur RSA_encrypt" << endl;

    oss << chif;

    return oss.str();


}*/

char* rsaCrypt::chiffrement(string clair){
    cout << "chiffrement rsa" << endl;
    cout << "laforeignkey" << forgeignKey << endl;
    ostringstream oss;
    unsigned char *chif = (unsigned char*)malloc(1024*sizeof(char));
    int succ;
    int kLen = RSA_size(forgeignKey) - 11;
    succ = RSA_public_encrypt(kLen,(const unsigned char *)clair.c_str(),chif,forgeignKey,RSA_PKCS1_PADDING);
    if(succ <= 0)
            cout << "erreur RSA_encrypt" << endl;

    return (char*) chif;


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

//string rsaCrypt::dechiffrement(string chif){

    // DECODAGE BASE64
/*	char *buffer=(char *)malloc(1024*sizeof(char));
    BIO *bio, *b64;

    
    bio = BIO_new_mem_buf((char *)chif.c_str(), -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64,bio);
    
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_read(bio, buffer, chif.length());
    
    BIO_free_all(bio);

*/
 /*   cout << "dechiffrement" << endl;
    ostringstream oss;
    unsigned char *clair=(unsigned char*)malloc(1024*sizeof(char));;
    int succ;
    int kLen = RSA_size(rsaKey);

	cout << chif << endl;	

    succ = RSA_private_decrypt(kLen,(const unsigned char *)chif.c_str(),clair,rsaKey,RSA_PKCS1_PADDING);

    if (succ < 0)
        cout << "erreur dechiffrement rsa" << endl;
   
    oss << clair;
    free(clair);
//    free(buffer);
    return oss.str();
 
}*/

string rsaCrypt::dechiffrement(const char * chif){

    cout << "dechiffrement" << endl;
    ostringstream oss;
    unsigned char *clair=(unsigned char*)malloc(1024*sizeof(char));;
    int succ;
    int kLen = RSA_size(rsaKey);

    succ = RSA_private_decrypt(kLen,(const unsigned char *)chif,clair,rsaKey,RSA_PKCS1_PADDING);

    if (succ < 0)
        cout << "erreur dechiffrement rsa" << endl;
   
    oss << clair;
    free(clair);
//    free(buffer);

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

     msg << id ;
     msg << "*" ;
     msg << "304*"; 
     msg << key; 
     msg << "*EOF";

     return msg.str();
}

bool rsaCrypt::recupKeyPub(string key){
    istringstream iss;
    //string bddKey;
    ostringstream oss;
    BIO *keybio = NULL ;
   /* oss << id;
    iss << key;
	cout << key << endl;
    getline(iss,bddKey,'*');

    if(!bddKey.compare(oss.str())){
        getline(iss,bddKey,'*');

        if(!bddKey.compare("init")){
            getline(iss,bddKey,'*');
            cout << bddKey << endl;*/

            keybio = BIO_new_mem_buf ((unsigned char *)key.c_str() , -1 );

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
    ostringstream bufferchif;    

    oss << id;
    iss.str(key);

    getline(iss,bddKey,'*');

    if(!bddKey.compare(oss.str())){
        getline(iss,bddKey,'*');

        if(!bddKey.compare("304")){
            getline(iss,bddKey,'*');
            bufferchif << bddKey;
	    getline(iss,bddKey,'*');
            while(bddKey!="EOF"){
		bufferchif << "*";
		bufferchif << bddKey;
		getline(iss,bddKey,'*');
		}
	    cout << "debut chiffré :" + bufferchif.str() + "fin chiffré" << endl;		

            istringstream issAes;
            string bddaAes;

            issAes.str(this->dechiffrement(bufferchif.str().c_str()));

            getline(issAes,bddaAes,';');
            aesKey = (char *)bddaAes.c_str();
		cout << "cle aes :";
		cout << aesKey << endl;
            getline(issAes,bddaAes,'*');
            aesIv = (char *)bddaAes.c_str();
		cout << "iv aes :"; 
		cout << aesIv << endl;
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
