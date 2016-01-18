#ifndef TSLOT_H
#define TSLOT_H

#include <iostream>
#include <boost/signals2.hpp>
#include <boost/timer/timer.hpp>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <QtNetwork>
#include <ctime>

#include <map>
#include <list>

class IpPort {
	public:
	QHostAddress hostAddress;
	quint16 port;
};

class Tslot
{
	private:
	boost::thread* m_thread;
	time_t t0;
	 
	bool m_mustStop;
	boost::mutex m_mustStopMutex;
	std::string m_lastToken;

	std::map<std::string, IpPort> tokenToClient;
	std::map<std::string, std::list<std::string>* > tokenToMsgList;
	std::map<std::string, std::list<char* >* > tokenToCharList;
	std::map<std::string, std::list<int >* > tokenToSizeList;
	std::map<std::string, boost::mutex* > tokenToMutex;
	std::map<std::string, bool> tokenToBool;

	public:

	Tslot();
	 
	~Tslot();

	std::string HexFormate(const unsigned char *hash, size_t length);

	std::string GenToken(QHostAddress addr, quint16 port);

	bool TriggerToken(std::string& token);

	void addMessageToList(std::string token, std::string msg);

	void addCharToList(std::string token, char * buf, int size);

	void addSizeToList(std::string token, int size);

	void printMessageToList(std::string token);

	std::list<int>* getListSize(std::string token);

	void timeoutCallback(std::string token);

	std::string getLastToken();
	boost::mutex * getMutex(std::string token);
	bool getBool(std::string token);
	 

	void Start();
	 

	void Stop();
	 
	void operator () ();

	std::list<std::string>* startTimer(std::string token);

	std::list<std::string>* startTimer(std::string token, int ms);

	std::list<char* >* startTimer_c(std::string token, int ms);
};

#endif