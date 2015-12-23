#ifndef FUSIONNEUR_H
#define FUSIONNEUR_H

#include <QtNetwork>
#include <timertoken.h>
#include <map>
#include <string>
#include <list>

class IpPort {
	public:
	QHostAddress hostAddress;
	quint16 port;
};

class Fusionneur : public QObject
{
		QOBJECT_H
	protected:
		Fusionneur();
		std::map<std::string, IpPort> tokenToClient;
		std::map<std::string, std::list<std::string>* > tokenToMsgList;
		std::map<std::string, QTimer*> tokenToTimer;
		static Fusionneur* _instance;
		time_t t0;
	public:
		static Fusionneur *getInstance();
		std::string GenToken(QHostAddress addr, quint16 port);
		std::list<std::string>* getReponses(std::string token);
		void startTimer(std::string token);
		void addMessageToList(std::string token, std::string msg);
	public slots:
		void timeoutCallback(std::string token);
};

#endif // FUSIONNEUR_H
