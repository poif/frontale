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

using namespace std;

class reception : public QObject
{
    Q_OBJECT
private :
    network_interface * m_netinf;
    Tslot * m_ts;
    QUdpSocket *soc;
    QString msg;
    bool expiration;
    QHostAddress hostAddr;
    quint16 hostPort;
    list<string> fileMsg;
public:
    reception(Tslot * ts, network_interface * netinf = NULL);
    void ecoute(quint16 port);
    string getFileMsg();
    QString getMsg();
    bool getExpiration();
    QHostAddress getHostAddr();
    quint16 getHostPort();
public slots:
        void procReception();
};

#endif // RECEPTION_H
