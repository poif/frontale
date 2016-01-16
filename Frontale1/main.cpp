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
#include "rsacrypt.h"

#include "network_interface.h"
#include "reception.h"
#include "message.h"
#include "lecture.h"


using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);


	lecture lec;
	
	Tslot ts;
	Tslot ts_s;

    Message mess;

	bdd_tcp bdd;
	bdd.connection_tcp(QString(lec.getAdresse_bdd().c_str()),lec.getPort_bdd());

    rsaCrypt rsabdd = rsaCrypt(2048);
    rsabdd.keyGen();
    
    do{
    bdd.emission(rsabdd.sendKeyPub(0), 1);

    bdd.attendLecture(30000, 1);

    }while(!bdd.getYLecture());
    
    rsabdd.recupAesKey(bdd.getMsg(), 0);

    bdd.setKeyIv(rsabdd.getAesKey(),rsabdd.getAesIv());

	reception res(&ts_s, &bdd, &mess);

    network_interface netinf(&bdd, &ts, &ts_s, &res, &mess);
    	std::thread tspawn = netinf.spawnThread();
 
    reception ser(&ts, &bdd, &mess, &netinf);

	ser.ecoute();

	res.ecoute_s();

	return a.exec();
}
