#ifndef H__network_interfaces
#define H__network_interfaces

#include <string>

#include <crypto++/rsa.h>
#include <crypto++/osrng.h>
#include <crypto++/base64.h>
#include <crypto++/files.h>


#include <queue>
#include <string>
#include <list>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/mutex.hpp>

#include "engine_event.h"

#define BUFFER_SIZE 2048

using namespace CryptoPP;

class network_interface{
public:
	network_interface();
	~network_interface();

	void frame();

	void send_eventUDP(engine_event& ne, boost::asio::ip::udp::socket* s);

	//void clean_interface();

	inline bool is_running()	{return running;}
	void push_received_event(engine_event& e);

	// observer functions
	std::string send_look(std::string& affectation);
	std::string send_exist(std::string& statut);
	void* send_lookrec(std::string& dataType, std::string& statut);
	std::string send_pull(std::string& reference, std::string& groupeClient, int n, int nRemote, RSA::PublicKey& pubRemote);

	std::string encrypto_rsa(std::string& plain);
	std::string encrypto_rsa(std::string& plain, RSA::PublicKey pubRemote);
	std::string decrypto_rsa(std::string& cipher);
	std::string Pub_toB64string();
	std::string Pub_toB64string(RSA::PublicKey publicRemoteKey);

	std::string encrypto_aes(SecByteBlock key, byte* iv, std::string& plain);
	std::string decrypto_aes(SecByteBlock key, byte* iv, std::string& cipher);
	std::string* aesKeyToS(SecByteBlock key, byte* iv);
	byte* sToB(std::string plain);
	SecByteBlock sToSbb(std::string plain);


	inline int get_type(){return type;}

	int port_udp_reception;

private:

	void UDP_async_read(const boost::system::error_code& e, size_t);

	// mutex to protect the 'received envents' queue
	boost::mutex l_receive_queue;
	void process_event(engine_event&);
	std::queue<engine_event> received_events;
	void process_received_events_queue();
	void process_received_events(engine_event&);

	void get_config_data();

	boost::asio::ip::udp::socket *s_udp_in;

	boost::asio::ip::udp::endpoint udp_remote_endpoint;
	boost::asio::io_service io;
	// buffer to receive data
	boost::array<char, BUFFER_SIZE> network_buffer;

	RSA::PrivateKey privateKey;
	RSA::PublicKey publicKey;
	
	int type;
	int save_n;
	int save_nRemote;
	bool running;
	std::string host_rem;
    	std::string port_rem;
};

#endif
