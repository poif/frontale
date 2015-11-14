#ifndef H__network_interfaces
#define H__network_interfaces

#include <string>

#include <crypto++/rsa.h>
#include <crypto++/osrng.h>
#include <crypto++/base64.h>
#include <crypto++/files.h>
using namespace CryptoPP;

#include <queue>
#include <vector>
#include <string>
#include <list>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/mutex.hpp>

#include "engine_event.h"

using namespace std;

class network_interface{
public:
	network_interface();
	~network_interface();

	void frame();

    void send_eventUDP(engine_event&, boost::asio::ip::udp::socket*);

	void clean_interface();

	inline bool is_running()	{return running;}
	void push_received_event(engine_event&);

	// observer functions
    string* send_look(int, serializable_vector3df&, serializable_vector3df&, int);
	string send_exist(int, serializable_vector3df&, serializable_vector3df&, int);
	string* send_lookrec(std::string& dataType, std::string& statut);
	string send_pull(std::string& reference, std::string& groupeClient, int n, int nRemote, RSA::PublicKey& pubRemote);

	string encrypto_rsa(string& plain);
	string encrypto_rsa(string& plain, RSA::PublicKey pubRemote);
	string decrypto_rsa(string& cipher);
	string Pub_toB64string();

	string encrypto_aes(SecByteBlock key, bytes* iv, string& plain);
	string decrypto_aes(SecByteBlock key, byte* iv, string& cipher);
	string* aesKeyToS(SecByteBlock key, byte* iv);
	byte* sToB(string plain);
	SecByteBlock sToSbb(string plain);

	inline int get_type(){return type;}

	int port_udp_reception;

private:

    void UDP_async_read(const boost::asio::error_code&, size_t);

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
};

#endif
