#ifndef FUSIONNEUR_H
#define FUSIONNEUR_H

#include <QtNetwork>
#include <timertoken.h>
#include <map>
#include <string>
#include <list>

class IpPort {
	public:
		IpPort();
	QHostAddress hostAddress;
	quint16 port;
};

class Fusionneur : public QObject
{
		QOBJECT_H
	protected:
		Fusionneur();
		std::map<std::string, IpPort> tokenToClient;
		std::map<std::string, std::list<std::string>* > tokenMsgList;
		std::map<std::string, TimerToken*> tokenToTimer;
		static Fusionneur* _instance;
		time_t t0;
	public:
		static Fusionneur *getInstance();
		std::string GenToken();
		std::list<std::string>* getReponses(std::string token);
		void createDataStorage(std::string token);
		void startTimer(std::string token);
	public slots:
		void doNothingForTestingPurpose(std::string token);
};

#endif // FUSIONNEUR_H
