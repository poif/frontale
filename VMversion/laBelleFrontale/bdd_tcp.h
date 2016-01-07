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
    void attendLecture();
    void lecture();
    QString getMsg();
    bool getYLecture();

private:
    QTcpSocket soc;
    QString msg;
    bool yLecture;

};

#endif // BDD_TCP_H
