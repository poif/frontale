#include <QCoreApplication>
#include "rsacrypt.h"
#include "clientFront.h"
#include "reception.h"
#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //istringstream iss;

    clientFront cli;

    rsaCrypt rsa = rsaCrypt(2048);

    rsa.keyGen();

    cli.socBind();

    cli.emission("init*" + rsa.sendKeyPub(1) + "*EOF");

    reception recep;

    recep.ecoute();

    rsa.dechiffrement(recep.getMsg().c_str());


    return a.exec();
}
