#include "task.h"
#include <QTimer>
#include <iostream>
#include "fusionneur.h"

Task::Task(QObject *parent) :
	QObject(parent)
{
	mThread = new QThread(this);
	this->moveToThread(mThread);
	connect(mThread, SIGNAL(started()), this, SLOT(process()));
	mThread->start();
}

void Task::process()
{
	Fusionneur *f = Fusionneur::getInstance();
	QHostAddress a;
	std::string token = f->GenToken(a, 0);
	std::cout << token << std::endl;
	f->startTimer(token);
}
