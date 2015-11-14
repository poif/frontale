#ifndef CLIENT_H
#define CLIENT_H
#include <QCoreApplication>
#include <QTcpSocket>


class client: public QObject
{
public:
    client();
    void socBind();
    void emission(Qstring message);

private:
        QTcpSocket soc;
};

#endif // CLIENT_H
