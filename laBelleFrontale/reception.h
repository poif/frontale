#ifndef RECEPTION_H
#define RECEPTION_H
#include <QCoreApplication>
#include <QtNetwork>
#include <list>
#include <string>

using namespace std;

class reception : public QObject
{
    Q_OBJECT
private :
    QUdpSocket *soc;
    QString msg;
    bool expiration;
    QHostAddress hostAddr;
    quint16 hostPort;
    list<string> fileMsg;
public:
    reception();
    void ecoute(int timeout);
    list<string> getFileMsg();
    QString getMsg();
    bool getExpiration();
    QHostAddress getHostAddr();
    quint16 getHostPort();
public slots:
        void procReception();
};

#endif // RECEPTION_H
