#ifndef RSACRYPT_H
#define RSACRYPT_H
#include <openssl/rsa.h>
#include <QCoreApplication>



class rsaCrypt
{
public:
    rsaCrypt(int keylen);
    QString chiffrement(QString clair);
    QString dechiffrement(QString chif);
    QString clePub();
    void keyGen();

private:
    RSA *rsaKey;
    int keyLen;
    RSA *forgeignKey;
};

#endif // RSACRYPT_H
