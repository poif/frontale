#include <iostream>
#include <QtNetwork>
#include <QCoreApplication>
#include "task.h"
#include "fusionneur.h"
#include <unistd.h>

int main(int argc, char**argv){
	QCoreApplication a(argc, argv);
	Task *t = new Task();
	sleep(10);
	exit(-1);
	return a.exec();
}
