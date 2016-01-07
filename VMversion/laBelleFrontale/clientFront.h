#ifndef CLIENT_FRONT_H
#define CLIENT_FRONT_H
#include <QCoreApplication>
#include <QUdpSocket>

#include <QString>
#include <string>

class clientFront: public QObject
{
public:
    clientFront();
    void socBind();
    void emission(std::string message, QHostAddress hostAddr, quint16 hostPort);
    void emission(std::string message);

private:
        QUdpSocket *soc;
};

#endif // CLIENT_H
