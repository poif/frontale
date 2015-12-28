#ifndef CLIENT_FRONT_H
#define CLIENT_FRONT_H
#include <QCoreApplication>
#include <QUdpSocket>

#include <QString>

class clientFront: public QObject
{
public:
    clientFront();
    void socBind();
    void emission(QString message, QHostAddress hostAddr, quint16 hostPort);
    void emission(QString message);

private:
        QUdpSocket *soc;
};

#endif // CLIENT_H
