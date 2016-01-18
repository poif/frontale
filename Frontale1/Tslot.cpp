#include "Tslot.h"

using boost::timer::cpu_timer;
using boost::timer::cpu_times;
using boost::timer::nanosecond_type;
using namespace std;
 

Tslot::Tslot()
{
	m_thread=NULL;
	m_mustStop=false;
	m_lastToken="";
	t0=time(NULL);
	srand(t0);
}
	 
// Destructor
Tslot::~Tslot()
{
	if (m_thread!=NULL) delete m_thread;
}

string Tslot::HexFormate(const unsigned char *hash, size_t length)
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

string Tslot::GenToken(QHostAddress addr, quint16 port)
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
	if (tokenToCharList.find(token) == tokenToCharList.cend()){
		tokenToCharList.emplace(token, new list<char* >());
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToCharList n'est pas censé avoir une liste à cet emplacement avant d'avoir recu ce token" << endl;
		return "";
	}
	if (tokenToSizeList.find(token) == tokenToSizeList.cend()){
		tokenToSizeList.emplace(token, new list<int>());
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToSizeList n'est pas censé avoir une liste à cet emplacement avant d'avoir recu ce token" << endl;
		return "";
	}
	if (tokenToBool.find(token) == tokenToBool.cend()){
		tokenToBool.emplace(token, false);
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToBool n'est pas censé avoir un boolean à cet emplacement avant d'avoir recu ce token" << endl;
		return "";
	}
	if (tokenToMutex.find(token) == tokenToMutex.cend()){
		tokenToMutex.emplace(token, new boost::mutex);
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToBool n'est pas censé avoir un boolean à cet emplacement avant d'avoir recu ce token" << endl;
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
	cout << __FUNCTION__ << " appelée avec le token: " << token << " en argument" << std::endl;
	m_lastToken = token;
	return token;
}

bool Tslot::TriggerToken(string& token)
{

	cout << "Declenchement du token : " << token << endl;
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
		return false;
	}
	if (tokenToCharList.find(token) == tokenToCharList.cend()){
		tokenToCharList.emplace(token, new list<char* >());
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToCharList n'est pas censé avoir une liste à cet emplacement avant d'avoir recu ce token" << endl;
		return "";
	}
	if (tokenToSizeList.find(token) == tokenToSizeList.cend()){
		tokenToSizeList.emplace(token, new list<int>());
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToSizeList n'est pas censé avoir une liste à cet emplacement avant d'avoir recu ce token" << endl;
		return "";
	}
	if (tokenToBool.find(token) == tokenToBool.cend()){
		tokenToBool.emplace(token, false);
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToBool n'est pas censé avoir un boolean à cet emplacement avant d'avoir recu ce token" << endl;
		return false;
	}
	if (tokenToMutex.find(token) == tokenToMutex.cend()){
		tokenToMutex.emplace(token, new boost::mutex);
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur : tokenToMutex n'est pas censé avoir un mutex à cet emplacement avant d'avoir recu ce token" << endl;
		return false;
	}
	cout << __FUNCTION__ << " appelée avec le token: " << token << " en argument" << std::endl;
	cout << "Token déclenché avec succès." << endl; 
	m_lastToken = token;
	return true;
}

void Tslot::addMessageToList(string token, string msg)
{
	if (tokenToMsgList.find(token) != tokenToMsgList.cend()){
		auto b = std::find(tokenToMsgList[token]->begin(),tokenToMsgList[token]->end(),msg);
		if(b == tokenToMsgList[token]->end()){
			tokenToMsgList[token]->push_back(msg);
		}
	}
	else {
		cerr << "Erreur : on tente de rajoutter des messages alors que le token n'existe pas. Timeout dépassé ou tentative d'attaque." << endl;
	}
}

void Tslot::addCharToList(string token, char* buf, int size)
{
	if (tokenToCharList.find(token) != tokenToCharList.cend()){

		char * tokBuf = (char*)malloc(sizeof(char)*1024);
		int temp;
		if(size < 1024) temp = size;
		else temp = 1024;
		for (int i = 0; i < temp; ++i)
		{
			tokBuf[i] = buf[i];
		}
		tokBuf[temp]='\0';
		tokenToCharList[token]->push_back(tokBuf);
	}
	else {
		cerr << "Erreur : on tente de rajoutter des char alors que le token n'existe pas. Timeout dépassé ou tentative d'attaque." << endl;
	}
}

void Tslot::addSizeToList(string token, int size)
{
	if (tokenToSizeList.find(token) != tokenToSizeList.cend()){
		tokenToSizeList[token]->push_back(size);
	}
	else {
		cerr << "Erreur : on tente de rajoutter des size alors que le token n'existe pas. Timeout dépassé ou tentative d'attaque." << endl;
	}
}

void Tslot::printMessageToList(string token)
{
	if (tokenToMsgList.find(token) != tokenToMsgList.cend()){
		for (auto it : *tokenToMsgList[token]) cout << " elem : " << it << endl;
	}
	else {
		cerr << "Erreur : on tente de rajoutter des messages alors que le token n'existe pas. Timeout dépassé ou tentative d'attaque." << endl;
	}
}

void Tslot::timeoutCallback(string token)
{
	cout << __FUNCTION__ << " appelée avec le token: " << token << " en argument" << std::endl;
	std::list<string>* listeReponse = tokenToMsgList[token];
	std::list<char* >* listeReponse_c = tokenToCharList[token];
	std::list<int>* listeReponse_i = tokenToSizeList[token];
	tokenToBool[token] = true;

	cout << "Fin du traitement, affichage en cours..." << endl;

	for (auto it : *listeReponse) cout << it << endl;
	for (auto it : *listeReponse_c) cout << it << endl;
	for (auto it : *listeReponse_i) cout << it << endl;

	//tokenToMsgList.erase(token);

	//traitement_rep_client(listeReponse);
	//delete listeReponse;

	//IpPort ipPortDuClient = tokenToClient[token];
	//tokenToClient.erase(token);

	/* TODO */
	/*Faire l'envoi au client*/

	cout << "Le slot à été appelé correctement avec le token" << token << endl;
}

string Tslot::getLastToken()
{
	return m_lastToken;
}

boost::mutex* Tslot::getMutex(string token)
{
	return tokenToMutex[token];
}

bool Tslot::getBool(string token)
{
	return tokenToBool[token];
}
	 
// Start the threa
void Tslot::Start()
{
	// Create thread and start it with myself as argument.
	// Pass myself as reference since I don't want a copy
	m_thread=new boost::thread(boost::ref(*this));
}
	 
// Stop the thread
void Tslot::Stop()
{
	// Signal the thread to stop (thread-safe)
	m_mustStopMutex.lock();
	m_mustStop=true;
	m_mustStopMutex.unlock();
	 
	// Wait for the thread to finish.
	if (m_thread!=NULL) m_thread->join();
}
	 
// Thread function
void Tslot::operator () ()
{
	bool mustStop;
	int ite = 1;

	cout << "Traitement en cours... ";
		 
	do
	{
		addMessageToList(m_lastToken, "sloubi "+to_string(ite));
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
std::list<int>* Tslot::getListSize(std::string token){

	return tokenToSizeList[token];
}

std::list<string>* Tslot::startTimer(string token){

	//cout << "Armement du thread..." << endl;
	//Start();
	//cout << "Thread lancé" << std::endl;

	cout << "Lancement du timer..." << std::endl;
	nanosecond_type const timecap(2000 * 1000000LL);
	//nanosecond_type last(0);
	cpu_timer timer;
	bool more_transactions = true;
	cout << "Timer lancé" << std::endl;

	while (more_transactions)
	{
	  //fonction_qui_fait_un_truc();
	  cpu_times const elapsed_times(timer.elapsed());
	  nanosecond_type const elapsed(elapsed_times.system
	    + elapsed_times.user);
	  if (elapsed >= timecap)
	  {
	    //cout << "Arret du thread..." << endl;
	    //Stop();
	    //cout << "Thread arreté." << endl;
	    more_transactions = false;
	  }
	}

	timeoutCallback(token);

	return tokenToMsgList[token];

}

std::list<string>* Tslot::startTimer(string token, int ms){

	//cout << "Armement du thread..." << endl;
	//Start();
	//cout << "Thread lancé" << std::endl;

	cout << "Lancement du timer..." << std::endl;
	nanosecond_type const timecap(ms * 1000000LL);
	//nanosecond_type last(0);
	cpu_timer timer;
	bool more_transactions = true;
	cout << "Timer lancé" << std::endl;

	while (more_transactions)
	{
	  //fonction_qui_fait_un_truc();
	  cpu_times const elapsed_times(timer.elapsed());
	  nanosecond_type const elapsed(elapsed_times.system
	    + elapsed_times.user);
	  if (elapsed >= timecap)
	  {
	    //cout << "Arret du thread..." << endl;
	    //Stop();
	    //cout << "Thread arreté." << endl;
	    more_transactions = false;
	  }
	}

	timeoutCallback(token);

	return tokenToMsgList[token];

}

std::list<char* >* Tslot::startTimer_c(string token, int ms){

	//cout << "Armement du thread..." << endl;
	//Start();
	//cout << "Thread lancé" << std::endl;

	cout << "Lancement du timer..." << std::endl;
	nanosecond_type const timecap(ms * 1000000LL);
	//nanosecond_type last(0);
	cpu_timer timer;
	bool more_transactions = true;
	cout << "Timer lancé" << std::endl;

	while (more_transactions)
	{
	  //fonction_qui_fait_un_truc();
	  cpu_times const elapsed_times(timer.elapsed());
	  nanosecond_type const elapsed(elapsed_times.system
	    + elapsed_times.user);
	  if (elapsed >= timecap)
	  {
	    //cout << "Arret du thread..." << endl;
	    //Stop();
	    //cout << "Thread arreté." << endl;
	    more_transactions = false;
	  }
	}

	timeoutCallback(token);

	return tokenToCharList[token];

}