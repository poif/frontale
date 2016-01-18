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
	 

Tslot::~Tslot()
{
	if (m_thread!=NULL) delete m_thread;
}

string Tslot::HexFormate(const unsigned char *hash, size_t length)
{

	ostringstream os;
	os.fill('0');
	os << hex;

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



	if (tokenToMsgList.find(token) == tokenToMsgList.cend()){
		tokenToMsgList.emplace(token, new list<string>());
	}
	else {

		return "";
	}
	if (tokenToCharList.find(token) == tokenToCharList.cend()){
		tokenToCharList.emplace(token, new list<char* >());
	}
	else {

		return "";
	}
	if (tokenToSizeList.find(token) == tokenToSizeList.cend()){
		tokenToSizeList.emplace(token, new list<int>());
	}
	else {
		return "";
	}
	if (tokenToBool.find(token) == tokenToBool.cend()){
		tokenToBool.emplace(token, false);
	}
	else {
		return "";
	}
	if (tokenToMutex.find(token) == tokenToMutex.cend()){
		tokenToMutex.emplace(token, new boost::mutex);
	}
	else {
		return "";
	}
	if (tokenToClient.find(token) == tokenToClient.cend()){
		IpPort p;
		p.hostAddress=addr;
		p.port=port;
		tokenToClient.emplace(token, p);
	}
	else {
		return "";
	}
	m_lastToken = token;
	return token;
}

bool Tslot::TriggerToken(string& token)
{

	if (tokenToMsgList.find(token) == tokenToMsgList.cend()){
		tokenToMsgList.emplace(token, new list<string>());
	}
	else {
		return false;
	}
	if (tokenToCharList.find(token) == tokenToCharList.cend()){
		tokenToCharList.emplace(token, new list<char* >());
	}
	else {
		return "";
	}
	if (tokenToSizeList.find(token) == tokenToSizeList.cend()){
		tokenToSizeList.emplace(token, new list<int>());
	}
	else {
		return "";
	}
	if (tokenToBool.find(token) == tokenToBool.cend()){
		tokenToBool.emplace(token, false);
	}
	else {
		return false;
	}
	if (tokenToMutex.find(token) == tokenToMutex.cend()){
		tokenToMutex.emplace(token, new boost::mutex);
	}
	else {
		return false;
	}
	m_lastToken = token;
	return true;
}

void Tslot::addMessageToList(string token, string msg)
{
	if (tokenToMsgList.find(token) != tokenToMsgList.cend()){
		tokenToMsgList[token]->push_back(msg);
	}
	else {
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
	}
}

void Tslot::addSizeToList(string token, int size)
{
	if (tokenToSizeList.find(token) != tokenToSizeList.cend()){
		tokenToSizeList[token]->push_back(size);
	}
	else {
	}
}

void Tslot::printMessageToList(string token)
{
	if (tokenToMsgList.find(token) != tokenToMsgList.cend()){
		for (auto it : *tokenToMsgList[token]) cout << " elem : " << it << endl;
	}
	else {
	}
}

void Tslot::timeoutCallback(string token)
{
	std::list<string>* listeReponse = tokenToMsgList[token];
	std::list<char* >* listeReponse_c = tokenToCharList[token];
	std::list<int>* listeReponse_i = tokenToSizeList[token];
	tokenToBool[token] = true;


	for (auto it : *listeReponse) cout << it << endl;
	for (auto it : *listeReponse_c) cout << it << endl;
	for (auto it : *listeReponse_i) cout << it << endl;

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
	 

void Tslot::Start()
{

	m_thread=new boost::thread(boost::ref(*this));
}
	 

void Tslot::Stop()
{

	m_mustStopMutex.lock();
	m_mustStop=true;
	m_mustStopMutex.unlock();
	 
	if (m_thread!=NULL) m_thread->join();
}
	 

void Tslot::operator () ()
{
	bool mustStop;
	int ite = 1;
		 
	do
	{
		addMessageToList(m_lastToken, "sloubi "+to_string(ite));
		ite++;	 

		boost::this_thread::sleep(boost::posix_time::millisec(40));
			 
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

	nanosecond_type const timecap(2000 * 1000000LL);

	cpu_timer timer;
	bool more_transactions = true;


	while (more_transactions)
	{

	  cpu_times const elapsed_times(timer.elapsed());
	  nanosecond_type const elapsed(elapsed_times.system
	    + elapsed_times.user);
	  if (elapsed >= timecap)
	  {

	    more_transactions = false;
	  }
	}

	timeoutCallback(token);

	return tokenToMsgList[token];

}

std::list<string>* Tslot::startTimer(string token, int ms){


	nanosecond_type const timecap(ms * 1000000LL);

	cpu_timer timer;
	bool more_transactions = true;


	while (more_transactions)
	{

	  cpu_times const elapsed_times(timer.elapsed());
	  nanosecond_type const elapsed(elapsed_times.system
	    + elapsed_times.user);
	  if (elapsed >= timecap)
	  {
	    more_transactions = false;
	  }
	}

	timeoutCallback(token);

	return tokenToMsgList[token];

}

std::list<char* >* Tslot::startTimer_c(string token, int ms){



	nanosecond_type const timecap(ms * 1000000LL);

	cpu_timer timer;
	bool more_transactions = true;

	while (more_transactions)
	{
	  cpu_times const elapsed_times(timer.elapsed());
	  nanosecond_type const elapsed(elapsed_times.system
	    + elapsed_times.user);
	  if (elapsed >= timecap)
	  {

	    more_transactions = false;
	  }
	}

	timeoutCallback(token);

	return tokenToCharList[token];

}