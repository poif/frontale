#ifndef BDD_TCP_H
#define BDD_TCP_H
#include <QTcpSocket>
#include <QCoreApplication>
#include <QTcpServer>
#include <openssl/aes.h>


class bdd_tcp : public QObject
{
    Q_OBJECT

public:
    bdd_tcp();
    void connection_tcp(QString IP, int port);
    void emission(std::string texte, int type=0);
    void attendLecture(int timeout, int type=0);
    void lecture();
    std::string getMsg();
    bool getYLecture();
    std::string chiffrement(std::string clair);
    std::string dechiffrement(std::string chif);
    void setKeyIv(char *key, char *iv);

private:
    QTcpSocket soc;
    std::string msg;
    bool yLecture;
    char *key;
    char *iv;

};

#endif // BDD_TCP_H
