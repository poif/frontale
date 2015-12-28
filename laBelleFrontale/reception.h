#ifndef RECEPTION_H
#define RECEPTION_H
#include <QCoreApplication>
#include <QtNetwork>


class reception : public QObject
{
    Q_OBJECT
private :
    QUdpSocket *soc;
    QString msg;
    bool expiration;
    QHostAddress hostAddr;
    quint16 hostPort;
public:
    reception();
    void ecoute(int timeout);
    void procReception();
    QString getMsg();
    bool getExpiration();
    QHostAddress getHostAddr();
    quint16 getHostPort();
};

#endif // RECEPTION_H
