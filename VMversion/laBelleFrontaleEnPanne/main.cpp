#include <QCoreApplication>
#include <message.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <list>
#include <unistd.h>
#include <QTextStream>
#include <string>
#include <QtNetwork>

#include "network_interface.h"
#include "reception.h"
#include "rsacrypt.h"
using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	
	Tslot ts;
	Tslot ts_s;

	bdd_tcp bdd;
	bdd.connection_tcp("127.0.0.1",2211);

	rsaCrypt rsabdd = rsaCrypt(2048);
	rsabdd.ketGen();

	do{
		bdd.emission(rsabdd.sendKeyPub(0),1);
		bdd.attendLecture(30000,1);

	}while(!bdd.getYLecture());

	rsabdd.recupAesKey(bdd.getMsg(),0);
	bdd.setKeyIv(rsabdd.getAesKey(), rsabdd.getAesIv());

	reception res(&ts_s, &bdd);

	network_interface netinf(&bdd, &ts, &ts_s, &res);
    	std::thread tspawn = netinf.spawnThread();
 
	reception ser(&ts, &bdd, &netinf);

	ser.ecoute();

	res.ecoute_s();

	return a.exec();
}
