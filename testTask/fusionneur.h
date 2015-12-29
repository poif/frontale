#ifndef FUSIONNEUR_H
#define FUSIONNEUR_H

#include <QtNetwork>
#include <map>
#include <string>
#include <list>
#include "timertoken.h"

class IpPort {
	public:
	QHostAddress hostAddress;
	quint16 port;
};

class Fusionneur : public QObject
{
		Q_OBJECT
	protected:
		Fusionneur();
		std::map<std::string, IpPort> tokenToClient;
		std::map<std::string, std::list<std::string>* > tokenToMsgList;
		std::map<std::string, TimerToken*> tokenToTimer;
		static Fusionneur* _instance;
		time_t t0;
		std::string HexFormate(const unsigned char * hash, size_t length);
	public:
		static Fusionneur *getInstance();
		std::string GenToken(QHostAddress addr, quint16 port);
		std::list<std::string>* getReponses(std::string token);
		void startTimer(std::string token);
		void startTimer(std::string token, int msec);
		void addMessageToList(std::string token, std::string msg);
	public slots:
		void timeoutCallback(std::string token);
};

#endif // FUSIONNEUR_H
