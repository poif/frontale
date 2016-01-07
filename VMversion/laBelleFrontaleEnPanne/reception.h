#ifndef RECEPTION_H
#define RECEPTION_H

#include <QCoreApplication>
#include <QtNetwork>
#include <list>
#include <string>
#include "network_interface.h"
#include "bdd_tcp.h"
#include "Tslot.h"
#include "clientFront.h"
#include "requete.h"
#include "message.h"

using namespace std;

class network_interface;
class Tslot;

class reception : public QObject
{
    Q_OBJECT
private :
    network_interface * m_netinf;
    Tslot * m_ts;
    bdd_tcp * m_bdd;
    QUdpSocket *soc;
    QString m_msg;
    bool expiration;
    QHostAddress hostAddr;
    quint16 hostPort;
    list<string> fileMsg;
public:
    reception(Tslot * ts, bdd_tcp * bdd,  network_interface * netinf = NULL);
    void ecoute();
    void ecoute_s();
    string getFileMsg();
    QString getMsg();
    bool getExpiration();
    void traitement(std::string messageStr);

    QHostAddress getHostAddr();
    quint16 getHostPort();
public slots:
        void procReception();
        void procReception_s();
};

#endif // RECEPTION_H
