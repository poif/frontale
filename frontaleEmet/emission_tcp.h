#ifndef EMISSION_TCP_H
#define EMISSION_TCP_H
#include <QTcpSocket>
#include <QCoreApplication>


class emission_tcp : public QObject
{
    Q_OBJECT

public:
    emission_tcp();
    void connection_tcp(QString IP, int port);
    void emission(QString texte);

private:
    QTcpSocket soc;

};

#endif // EMISSION_TCP_H
