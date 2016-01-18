#include "message.h"
#include <QCoreApplication>
#include <openssl/aes.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <QTextStream>
#include <sstream>
#include "rsacrypt.h"
#include "clientFront.h"

Message::Message(QString msg, char type, char separateur)
{
    this->msg = msg;
    this->type = type;
    this->separateur = separateur;
    this->nbKey=0;
    toSendArray = (char*)malloc(1024*sizeof(char));
}

Message::Message(QString chiffre, char separateur){
    this->chiffre = chiffre;
    this->separateur = separateur;
    this->nbKey=0;
    toSendArray = (char*)malloc(1024*sizeof(char));
}

Message::Message(){
    this->nbKey=0;
    toSendArray = (char*)malloc(1024*sizeof(char));
 
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

char* Message::crypt(unsigned char* aes_input, int size_aes_input, int numkey ){
    
    unsigned char trame [1024];
 
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    AES_KEY enc_key;
    AES_set_encrypt_key((const unsigned char *)tabKeyIv[numkey][0].c_str(), 256, &enc_key);
    AES_cbc_encrypt(aes_input, trame, size_aes_input, &enc_key,iv, AES_ENCRYPT);
 
    return (char *) trame;

 }

std::string Message::decrypt(unsigned char* dec_in, int size_aes_input){

    unsigned char trame [1024];
    istringstream iss;
    string chaineTest;
    int i= 0;
    int newKey;
    string aesToSend;
    
     while(chaineTest != "chall" && i<nbKey && !tabKeyIv[i][0].empty() && !tabKeyIv[i][0].empty()){

        AES_KEY dec_key;
        
        AES_set_decrypt_key((const unsigned char *)tabKeyIv[i][0].c_str(), 256, &dec_key);

        unsigned char iv[AES_BLOCK_SIZE];
        memset(iv, 0x00, AES_BLOCK_SIZE);
        AES_cbc_encrypt(dec_in, trame, size_aes_input, &dec_key,iv, AES_DECRYPT);

  

        cout << trame << endl;

        iss.str(string((const char *)trame));
        getline(iss,chaineTest,'*');

        if(chaineTest != "chall") i++;
    }
    cout << nbKey << endl;
    cout << i << endl;


    if(nbKey==i){


	
       strncpy((char *)trame, reinterpret_cast<const char*>(dec_in), sizeof(trame));
        iss.str(string((const char *)trame));
        getline(iss,chaineTest,'*');
        string lakey;
        getline(iss,lakey,'*');

        if(chaineTest=="init"){



		
            rsaCrypt rsaClient = rsaCrypt(2048);
            rsaClient.recupKeyPub(lakey);
            newKey = this->genAESKey();
            aesToSend = tabKeyIv[newKey][0]+";"+tabKeyIv[newKey][1];


            
            char *temp;

            temp = rsaClient.chiffrement(aesToSend);

            toSendArray[0] = '5';
            toSendArray[1] = '*';

            for(int k = 2;k<1024;k++) toSendArray[k] = temp[k-2];

            eChangeKey=true;
            numClient=i;
        }else {
            dechiffre = false;

        }
        return iss.str();

    }else{
        string reponse;
        getline(iss,reponse,'\0');

        dechiffre = true;
        eChangeKey=false;
	numClient = i;
        return reponse;


    }

 }


bool Message::dechiffrement(const unsigned char *key){
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
    return true;
}


int Message::genAESKey(){
    int i ;
    ostringstream oss;
    ostringstream oss2;


    char *key = (char *) malloc ( (AES_KEY_LENGTH+1)*sizeof(char) ) ;
    char *iv =(char *) malloc ((AES_KEY_LENGTH+1)*sizeof(char));

    memset ( key, '\0', AES_KEY_LENGTH +1) ;
    memset ( iv, '\0', AES_KEY_LENGTH +1) ;

 
    for ( i = 0; i < AES_KEY_LENGTH ; i++ )
    {
        key[i] = rand() % ( 122 - 97 ) + 97 ;
        iv[i] = rand() % ( 122 - 97 ) + 97 ;
    }
    oss << key;

    tabKeyIv[nbKey][0] = oss.str();

    oss2 << iv;

    tabKeyIv[nbKey][1] = oss2.str();

    nbKey++;

    return nbKey-1;

}

string Message::getAESKey(int numKey){
    return tabKeyIv[numKey][0];
}

string Message::getAESIv(int numKey){
    return tabKeyIv[numKey][1];
}

int Message::getNumClient(){
    return numClient;
}

string Message::getToSend(){
    return toSend;
}

char* Message::getToSendArray(){
    return toSendArray;
}

bool Message::getDechiffre(){
    return dechiffre;
}

bool Message::getEChangeKey(){
    return eChangeKey;
}

int Message::getNbKey(){
    return nbKey;
}
