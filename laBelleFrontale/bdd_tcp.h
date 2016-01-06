#ifndef BDD_TCP_H
#define BDD_TCP_H
#include <QTcpSocket>
#include <QCoreApplication>
#include <QTcpServer>
#include <openssl/aes.h>
#include <QCoreApplication>

class bdd_tcp : public QObject
{
    Q_OBJECT

public:
    bdd_tcp();
    void connection_tcp(QString IP, int port);
    void emission(string texte, int type=0);
    void attendLecture(int timeout, int type=0);
    void lecture();
    std::string getMsg();
    bool getYLecture();
    QString chiffrement(std::string clair);
    std::string dechiffrement(QString chif);
    void setKeyIv(char *key, char *iv);

private:
    QTcpSocket soc;
    QString msg;
    bool yLecture;
    char *key;
    char *iv;

};

#endif // BDD_TCP_H
