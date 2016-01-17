#ifndef MESSAGE_H
#define MESSAGE_H
#define AES_KEY_LENGTH      32
#include <iostream>
#include <QCoreApplication>
#include <string>

using namespace std;

#define NB_MAX_CLIENT 1000
class Message
{
private:
    QString msg;
    char type;
    char separateur;
    QString chiffre;
    string tabKeyIv[NB_MAX_CLIENT][2];
    int nbKey;
    int numClient;
    string toSend;
    char * toSendArray;
    bool eChangeKey;
    bool dechiffre;

public:
    Message(QString msg, char type, char separateur);
    Message(QString chiffre, char separateur);
    Message();
    QString getMsg();
    QString getChiffre();
    void chiffrement(const unsigned char *key);
    void entete();
    void challenge(int N);
    bool dechiffrement(const unsigned char *key);
    string decrypt(unsigned char* dec_in, int size_aes_input);
    string crypt(unsigned char* aes_input, int size_aes_input, int numkey);
    int genAESKey();
    int getNumClient();
    string getToSend();
    char* getToSendArray();
    bool getEChangeKey();
    bool getDechiffre(); 
    string getAESKey(int numKey);
    string getAESIv(int numKey);
    int getNbKey();

};

#endif // MESSAGE_H
