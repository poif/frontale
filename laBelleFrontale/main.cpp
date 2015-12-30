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

using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	
	Tslot ts;
	reception res(&ts);

	bdd_tcp bdd;
	bdd.connection_tcp("127.0.0.1",2211);

	network_interface netinf(&bdd, &ts, &res);
    	std::thread tspawn = netinf.spawnThread();
 
	reception ser(&ts, &netinf);

	ser.ecoute(12345);

	res.ecoute(54321);

	return a.exec();
}