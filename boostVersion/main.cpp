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
//#include "fusionneur.h"

using boost::timer::cpu_timer;
using boost::timer::cpu_times;
using boost::timer::nanosecond_type;
using namespace std;

class IpPort {
	public:
	QHostAddress hostAddress;
	quint16 port;
};

class ReponseClass
{
	private:
	boost::thread* m_thread; // The thread runs this object
	time_t t0;
	string m_token;
	 
	// Variable that indicates to stop and the mutex to
	// synchronise "must stop" on (mutex explained later)
	bool m_mustStop;
	boost::mutex m_mustStopMutex;

	std::map<std::string, IpPort> tokenToClient;
	std::map<std::string, std::list<std::string>* > tokenToMsgList;	 

	public:
	// Default constructor
	ReponseClass()
	{
		m_thread=NULL;
		m_mustStop=false;
		m_token="";
		t0=time(NULL);
		srand(t0);
	}
	 
	// Destructor
	~ReponseClass()
	{
		if (m_thread!=NULL) delete m_thread;
	}

	string HexFormate(const unsigned char *hash, size_t length)
	{
		//init OSS
		ostringstream os;
		os.fill('0');
		os << hex;
		//parcours de la chaine et copie de 2 caractères hexa
		for(const unsigned char * ptr = hash ; ptr < hash + length ; ptr++)
			os << setw(2) << (unsigned int) *ptr;

		return os.str();
	}

	string GenToken(QHostAddress addr, quint16 port)
	{
		ostringstream tokenContainer;
		tokenContainer << time(NULL)-t0<< rand()%100;

		SHA_CTX ctx;
		SHA1_Init(&ctx);
		char hashChar[SHA_DIGEST_LENGTH];
		SHA1_Update(&ctx,tokenContainer.str().c_str(),tokenContainer.str().length());
		SHA1_Final((unsigned char*)hashChar,&ctx);
		string token = HexFormate((const unsigned char*) hashChar, SHA_DIGEST_LENGTH);

		cout << "TOKEN GENERE : " << token << endl;
		/*if (tokenToTimer.find(token) == tokenToTimer.cend()){
			tokenToTimer.emplace(token, new TimerToken(token));
			tokenToTimer[token]->setInterval(2000);
			tokenToTimer[token]->setSingleShot(true);
			cout << "Resultat du connect" << connect(tokenToTimer[token], SIGNAL(timeOutMyToken(std::string)), this, SLOT(timeoutCallback(std::string))) << endl;
		}
		else {
			cerr << "Erreur : tokenToTimer n'est pas censé avoir un TimerToken à cet emplacement avant d'avoir recu ce token" << endl;
			return "";
		}*/
		if (tokenToMsgList.find(token) == tokenToMsgList.cend()){
			tokenToMsgList.emplace(token, new list<string>());
		}
		else {
			// Ne devrait jamais s'afficher
			cerr << "Erreur : tokenToMsgList n'est pas censé avoir une liste à cet emplacement avant d'avoir recu ce token" << endl;
			return "";
		}
		if (tokenToClient.find(token) == tokenToClient.cend()){
			IpPort p;
			p.hostAddress=addr;
			p.port=port;
			tokenToClient.emplace(token, p);
		}
		else {
			cerr << "Erreur : tokenToClient n'est pas censé avoir un TimerToken à cet emplacement avant d'avoir recu ce token" << endl;
			return "";
		}
		m_token = token;
		return token;
	}

	void addMessageToList(string token, string msg)
	{
		if (tokenToMsgList.find(token) != tokenToMsgList.cend()){
			tokenToMsgList[token]->push_back(msg);
		}
		else {
			cerr << "Erreur : on tente de rajoutter des messages alors que le token n'existe pas. Timeout dépassé ou tentative d'attaque." << endl;
		}
	}

	void printMessageToList(string token)
	{
		if (tokenToMsgList.find(token) != tokenToMsgList.cend()){
			for (auto it : *tokenToMsgList[token]) cout << " elem : " << it << endl;
		}
		else {
			cerr << "Erreur : on tente de rajoutter des messages alors que le token n'existe pas. Timeout dépassé ou tentative d'attaque." << endl;
		}
	}

	void timeoutCallback(string token)
	{
		cout << __FUNCTION__ << " appelée avec le token: " << token << " en argument" << std::endl;
		std::list<string>* listeReponse = tokenToMsgList[token];

		cout << "Fin du traitement, affichage en cours..." << endl;

		for (auto it : *listeReponse) cout << it << endl;

		tokenToMsgList.erase(token);

		//traitement_rep_client(listeReponse);
		delete listeReponse;

		IpPort ipPortDuClient = tokenToClient[token];
		tokenToClient.erase(token);

		/* TODO */
		/*Faire l'envoi au client*/

		cout << "Le slot à été appelé correctement avec le token" << token << endl;
	}

	string getToken()
	{
		return m_token;
	}
	 
	// Start the threa
	void Start()
	{
		// Create thread and start it with myself as argument.
		// Pass myself as reference since I don't want a copy
		m_thread=new boost::thread(boost::ref(*this));
	}
	 
	// Stop the thread
	void Stop()
	{
		// Signal the thread to stop (thread-safe)
		m_mustStopMutex.lock();
		m_mustStop=true;
		m_mustStopMutex.unlock();
	 
		// Wait for the thread to finish.
		if (m_thread!=NULL) m_thread->join();
	}
	 
	// Thread function
	void operator () ()
	{
		bool mustStop;
		int ite = 1;

		cout << "Traitement en cours... ";
		 
		do
		{
			addMessageToList(m_token, "sloubi "+to_string(ite));
			ite++;
			 
			// Sleep for 40ms (25 frames/second).
			boost::this_thread::sleep(boost::posix_time::millisec(40));
			 
			// Get the "must stop" state (thread-safe)
			m_mustStopMutex.lock();
			mustStop=m_mustStop;
			m_mustStopMutex.unlock();
		}
		while (mustStop==false);

		cout << endl;

	}

	void startTimer(string token){

		cout << "Armement du thread..." << endl;
		Start();
		cout << "Thread lancé" << std::endl;

		cout << "Lancement du timer..." << std::endl;
		nanosecond_type const two_seconds(2 * 1000000000LL);
		nanosecond_type last(0);
		cpu_timer timer;
		bool more_transactions = true;
		cout << "Timer lancé" << std::endl;

		while (more_transactions)
		{
		  //fonction_qui_fait_un_truc();
		  cpu_times const elapsed_times(timer.elapsed());
		  nanosecond_type const elapsed(elapsed_times.system
		    + elapsed_times.user);
		  if (elapsed >= two_seconds)
		  {
		    cout << "Arret du thread..." << endl;
		    Stop();
		    cout << "Thread arreté." << endl;
		    more_transactions = false;
		  }
		}

		timeoutCallback(token);

	}
};

int main(){
 
	ReponseClass rc;

	string token;

	cout << "Génération du token..." << endl;
	token = rc.GenToken(QHostAddress("127.0.0.1"),8173);
	cout << "Token généré." << endl;

 	rc.startTimer(token);
	cout << "Tout nu !" << endl;

	return 0;

}