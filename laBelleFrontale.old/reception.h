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

public:
    reception();
    void ecoute(int timeout);
    void procReception();
    QString getMsg();
};

#endif // RECEPTION_H
