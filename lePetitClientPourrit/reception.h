#ifndef RECEPTION_H
#define RECEPTION_H

#include <QCoreApplication>
#include <QtNetwork>
#include <list>
#include <string>


using namespace std;

class network_interface;
class Tslot;

class reception : public QObject
{
    Q_OBJECT
private :

    QUdpSocket *soc;
    string chiffre;


public:
    reception();
    void ecoute();
    string getMsg();
    void procReception();
};

#endif // RECEPTION_H
