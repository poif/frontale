#ifndef AESBDD_H
#define AESBDD_H
#include <QCoreApplication>
#include <openssl/aes.h>
#include <iostream>
#include <string>

using namespace std;

class aesBdd
{
public:
    aesBdd(char *key, char *iv);
    QString chiffrement(string clair);
    string dechiffrement(QString chif);

private:
    char *key;
    char *iv;
};

#endif // AESBDD_H
