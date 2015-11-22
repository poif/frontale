#ifndef CLIENT_H
#define CLIENT_H
#include <QCoreApplication>
#include <QUdpSocket>

#include <QString>

class client: public QObject
{
public:
    client();
    void socBind();
    void emission(QString message);

private:
        QUdpSocket *soc;
};

#endif // CLIENT_H
