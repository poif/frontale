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
	boost::thread* m_thread; // The thread runs this object
	time_t t0;
	std::string m_token;
	 
	// Variable that indicates to stop and the mutex to
	// synchronise "must stop" on (mutex explained later)
	bool m_mustStop;
	boost::mutex m_mustStopMutex;

	std::map<std::string, IpPort> tokenToClient;
	std::map<std::string, std::list<std::string>* > tokenToMsgList;
	//std::map<std::string, TimerToken*> tokenToTimer;	 

	public:
	// Default constructor
	Tslot();
	 
	// Destructor
	~Tslot();

	std::string HexFormate(const unsigned char *hash, size_t length);

	std::string GenToken(QHostAddress addr, quint16 port);

	void addMessageToList(std::string token, std::string msg);

	void printMessageToList(std::string token);

	void timeoutCallback(std::string token);

	std::string getToken();
	 
	// Start the threa
	void Start();
	 
	// Stop the thread
	void Stop();
	 
	// Thread function
	void operator () ();

	void startTimer(std::string token);

	void startTimer(std::string token, int ms);
};

#endif