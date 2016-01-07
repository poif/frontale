#ifndef MESSAGE_H
#define MESSAGE_H
#include <iostream>
#include <QCoreApplication>

class Message
{
private:
    QString msg;
    char type;
    char separateur;
    QString chiffre;


public:
    Message(QString msg, char type, char separateur);
    Message(QString chiffre, char separateur);
    QString getMsg();
    QString getChiffre();
    void chiffrement(const unsigned char *key);
    void entete();
    void challenge(int N);
    void dechiffrement(const unsigned char *key);
    std::string decrypt(unsigned char* dec_in, int size_aes_input);
    std::string crypt(unsigned char* aes_input, int size_aes_input);


};

#endif // MESSAGE_H