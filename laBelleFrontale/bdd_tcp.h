#ifndef BDD_TCP_H
#define BDD_TCP_H
#include <QTcpSocket>
#include <QCoreApplication>
#include <QTcpServer>

class bdd_tcp : public QObject
{
    Q_OBJECT

public:
    bdd_tcp();
    void connection_tcp(QString IP, int port);
    void emission(QString texte);
    void attendLecture(int timeout);
    void lecture();
    QString getMsg();
    bool getYLecture();
    QString chiffrement(string clair);
    string dechiffrement(QString chif);
    void setKeyIv(char *key, char *iv);

private:
    QTcpSocket soc;
    QString msg;
    bool yLecture;
    char *key;
    char *iv;

};

#endif // BDD_TCP_H
