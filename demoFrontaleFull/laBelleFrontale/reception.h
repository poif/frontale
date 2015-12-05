#ifndef SERVER_TCP_H
#define SERVER_TCP_H
#include <QCoreApplication>
#include <QtNetwork>


class reception : public QObject
{
    Q_OBJECT
private :
    QUdpSocket *soc;

public:
    reception();
    void ecoute();

private slots:
    void procReception();
};

#endif // SERVER_TCP_H
