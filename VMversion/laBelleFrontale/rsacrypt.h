#ifndef RSACRYPT_H
#define RSACRYPT_H
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <QCoreApplication>



class rsaCrypt
{
public:
    rsaCrypt(int keylen);
    QString chiffrement(QString clair);
    QString dechiffrement(QString chif);
    QString sendKeyPub(int id);
    bool recupKeyPub(QString key, int id);
    bool recupAesKey(QString key, int id );
    void keyGen();
    char * getAesKey();
    char * getAesIv();

private:
    RSA *rsaKey;
    int keyLen;
    RSA *forgeignKey;
    char *aesKey;
    char *aesIv;
};

#endif // RSACRYPT_H
