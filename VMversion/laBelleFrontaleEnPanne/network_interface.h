#ifndef H__network_interfaces
#define H__network_interfaces

#include <string>

#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <thread>

#include <queue>
#include <string>
#include <list>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/mutex.hpp>

#include "engine_event.h"
#include "noeudthor.h"
#include "bdd_tcp.h"
#include "Tslot.h"
#include "reception.h"

#define BUFFER_SIZE 2048

using namespace CryptoPP;

class NoeudThor;
class reception;

class network_interface{
public:
	network_interface(bdd_tcp *outbdd,  Tslot *ourts, Tslot * ourts_s, reception * outres, Message * ourMess);
	~network_interface();

	void spawn();

	std::thread spawnThread() {
          		return std::thread([=] { spawn(); });
      	}

	void frame();

	//void send_eventUDP(engine_event& ne, boost::asio::ip::udp::socket* s);

	//void clean_interface();

	inline bool is_running()	{return running;}
	void push_received_event(engine_event& e);

	bool getRecbool();
	void setRecbool(bool reBool);
	std::string getResp();
	void setResp(string reResp);

	// observer functions
	void send_look(std::string& affectation, std::string& token);
	void send_exist(std::string& affectation, std::string& token);
	void send_lookrec(std::string& dataType, std::string& affectation, std::string& token);
	void send_pull(std::string& reference, std::string& groupeClient, std::string& encKey, std::string& token);

	std::string encrypto_rsa(std::string& plain);
	std::string encrypto_rsa(std::string& plain, CryptoPP::RSA::PublicKey pubRemote);
	std::string decrypto_rsa(std::string& cipher);
	std::string Pub_toB64string();
	std::string Pub_toB64string(CryptoPP::RSA::PublicKey publicRemoteKey);

	std::string encrypto_aes(SecByteBlock key, byte* iv, std::string& plain);
	std::string decrypto_aes(SecByteBlock key, byte* iv, std::string& cipher);
	std::string* aesKeyToS(SecByteBlock key, byte* iv);
	byte* sToB(std::string plain);
	SecByteBlock sToSbb(std::string plain);

	void tor_receive(std::string str_data);

	inline int get_type(){return type;}

	int port_udp_reception;

private:

	//pthread_t t;

	// mutex to protect the 'received envents' queue
	boost::mutex l_receive_queue;
	void process_event(engine_event&);
	std::queue<engine_event> received_events;
	void process_received_events_queue();

	std::string treat_resource(std::string& request, std::string& token, int type=0, std::string requetebdd="");
	void process_received_events(engine_event&);

	//void get_config_data();

	boost::asio::ip::udp::socket *s_udp_in;

	NoeudThor *noeudthor;

	boost::asio::ip::udp::endpoint udp_remote_endpoint;
	boost::asio::io_service io;
	// buffer to receive data
	boost::array<char, BUFFER_SIZE> network_buffer;

	CryptoPP::RSA::PrivateKey privateKey;
	CryptoPP::RSA::PublicKey publicKey;

	bdd_tcp * bdd;
	Tslot * ts;
	Tslot * ts_s;
	reception * res;
	Message * mess;
	
	
	int type;
	bool running;
	std::string host_rem;
    	int port_rem;
    	bool recBool;
    	std::string responseRec;

    	std::string aesKeyTampon;
    	std::string aesIvTampon;
};

#endif