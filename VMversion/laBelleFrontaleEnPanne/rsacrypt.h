#ifndef RSACRYPT_H
#define RSACRYPT_H
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <QCoreApplication>



class rsaCrypt
{
public:
    rsaCrypt(int keylen);
    std::string chiffrement(std::string clair);
    std::string dechiffrement(std::string chif);
    std::string sendKeyPub(int id);
    bool recupKeyPub(std::string key);
    bool recupAesKey(std::string key, int id );
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
