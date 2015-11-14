#include <asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <vector>
#include <iostream>

#include <string>

#include <crypto++/rsa.h>
#include <crypto++/aes.h>
#include <crypto++/osrng.h>
#include <crypto++/base64.h>
#include <crypto++/files.h>
#include <crypto++/modes.h>
#include <crypto++/filters.h>
#include <cryptopp/hex.h>

#include "network_interface.h"
#include "engine_event.h"
#include "network_machine.h"
#include "utils.h"

using namespace std;
using namespace CryptoPP;

/**
 *  very very bad way to avoid error during compilation
 *  created from boost/static_assert.hpp line 36
 */
template <> struct boost::STATIC_ASSERTION_FAILURE<false> { enum { value = 0 }; };

/**
 * constructor
 */
network_interface::network_interface(){
	engine_name = "network engine";
	tcp_acceptor = NULL;
	ni.reset();
	next_machine_id = 1;
	state = STATE_NOTHING;
	running = true;

	///////////////////////////////////////
	// Generate Parameters
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 3072);

	///////////////////////////////////////
	// Create Keys
	privateKey = RSA::PrivateKey(params);
	publicKey = RSA::PublicKey(params);

	// With the current version of Crypto++, MessageEnd() needs to be called
	// explicitly because Base64Encoder doesn't flush its buffer on destruction.
	Base64Encoder privkeysink(new FileSink("myprivkey_net.txt"));
	privateKey.DEREncode(privkeysink);
	privkeysink.MessageEnd();
	
	Base64Encoder pubkeysink(new FileSink("mypubkey_net.txt"));
	publicKey.DEREncode(pubkeysink);
	pubkeysink.MessageEnd();
	
	try{
		get_config_data();
	}catch(exception&e){
		cerr << e.what() << endl;
	}catch(string& e){
		// cannot create network, will continue without networking, no listening, no connect
		cerr << e << endl;
		running = false;
	}catch(...){
		cerr << "unknown exception handled" << endl;
	}

	clean_interface();
}


network_interface::~network_interface(){		
	boost::mutex::scoped_lock l(state_mutex);
		state = STATE_QUITTING;
	l.unlock();

	io.stop();
}


string network_interface::encrypto_rsa(string& plain){

	AutoSeededRandomPool rng;
	string cipher;

	////////////////////////////////////////////////
	// Encryption
	RSAES_OAEP_SHA_Encryptor e(publicKey);

	StringSource ss1(plain, true,
	    new PK_EncryptorFilter(rng, e,
	        new StringSink(cipher)
	   ) // PK_EncryptorFilter
	); // StringSource

	return cipher;

}

string network_interface::encrypto_rsa(string& plain, RSA::PublicKey pubRemote){

	AutoSeededRandomPool rng;
	string cipher;

	////////////////////////////////////////////////
	// Encryption
	RSAES_OAEP_SHA_Encryptor e(pubRemote);

	StringSource ss1(plain, true,
	    new PK_EncryptorFilter(rng, e,
	        new StringSink(cipher)
	   ) // PK_EncryptorFilter
	); // StringSource

	return cipher;

}

string network_interface::decrypto_rsa(string& cipher){

	AutoSeededRandomPool rng;
	string recovered;

	////////////////////////////////////////////////
	// Decryption
	RSAES_OAEP_SHA_Decryptor d(privateKey);

	StringSource ss2(cipher, true,
	    new PK_DecryptorFilter(rng, d,
	        new StringSink(recovered)
	   ) // PK_DecryptorFilter
	); // StringSource

	return recovered;

	

}
/*
void network_interface::generateAESkey(){
	AutoSeededRandomPool prng;

	SecByteBlock key(AES::DEFAULT_KEYLENGTH);
	prng.GenerateBlock( key, key.size() );

	byte iv[ AES::BLOCKSIZE ];
	prng.GenerateBlock( iv, sizeof(iv) );
}
*/
string network_interface::encrypto_aes(SecByteBlock key, bytes* iv, string& plain){

	string cipher;

	/*********************************\
	\*********************************/

	try
	{

	    CBC_Mode< AES >::Encryption e;
	    e.SetKeyWithIV( key, key.size(), iv );

	    // The StreamTransformationFilter adds padding
	    //  as required. ECB and CBC Mode must be padded
	    //  to the block size of the cipher.
	    StringSource ss( plain, true, 
	        new StreamTransformationFilter( e,
	            new StringSink( cipher )
	        ) // StreamTransformationFilter      
	    ); // StringSource
	}
	catch( const CryptoPP::Exception& e )
	{
	    cerr << e.what() << endl;
	    exit(1);
	}

	return cipher;

}

string network_interface::decrypto_aes(SecByteBlock key, byte* iv, string& cipher){

	string recovered;

	try
	{
	    CBC_Mode< AES >::Decryption d;
	    d.SetKeyWithIV( key, key.size(), iv );

	    // The StreamTransformationFilter removes
	    //  padding as required.
	    StringSource ss( cipher, true, 
	        new StreamTransformationFilter( d,
	            new StringSink( recovered )
	        ) // StreamTransformationFilter
	    ); // StringSource
	}
	catch( const CryptoPP::Exception& e )
	{
	    cerr << e.what() << endl;
	    exit(1);
	}

	return recovered;
	

}

string* network_interface::aesKeyToS(SecByteBlock key, byte* iv){
	string* aesString = new string[2];

	StringSource ss5( key.data(), key.size() , true, 
	        new HexEncoder(
	            new StringSink( aesString[0] )
	        ) // StreamTransformationFilter      
	); // StringSource

	StringSource ss6( iv, sizeof(iv) , true, 
	        new HexEncoder(
	            new StringSink( aesString[1] )
	        ) // StreamTransformationFilter      
	); // StringSource
}

byte* network_interface::sToB(string plain){

	byte * conv = new byte(plain.size()+1);
	conv = reinterpret_cast<unsigned char*>(const_cast<char*>(plain.data()));

	return conv;
}

SecByteBlock network_interface::sToSbb(string plain){

	SecByteBlock conv(reinterpret_cast<const unsigned char*>(plain.data()), plain.size());

	return conv;
}

void network_interface::get_config_data(){	

	port_udp_reception = 4447;
	
	// starts the network and set up all the async_callbacks
	// TODO : regarder configuration broadcast
	asio::ip::udp::endpoint udp_listening_ep(asio::ip::udp::v4(), port_udp_reception);
	s_udp_in = new asio::ip::udp::socket(io, udp_listening_ep);

	try{

	s_udp_in->async_receive_from(	asio::buffer(network_buffer, BUFFER_SIZE), 
									udp_remote_endpoint, 
									boost::bind(&network_interface::UDP_async_read, 
												this, 
												asio::placeholders::error,
												asio::placeholders::bytes_transferred)
								);
	}catch(exception&e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "unknown exception handled" << endl;
	}

	cout << "UDP listening " << s_udp_in->local_endpoint().address() << ":" << s_udp_in->local_endpoint().port()  << endl;

	asio::thread(boost::bind(&asio::io_service::run, &io));
}

void network_interface::frame(){
	// events to send (maybe not needed)
	// process_queue();

	// events received
	process_received_events_queue();
}

/**
 *  main reception function. it receives data from UDP connection
 *  we only accept messages from registered machines
 *  asynchronous function
 */
void network_interface::UDP_async_read(const asio::error_code& e, size_t){
	// check if the messages comes from a registered machine	
	/*if (!is_address_registered(udp_remote_endpoint.address().to_string()))
		return;*/
	
	// let's deserialize the message
	string str_data(&network_buffer[0], network_buffer.size());
	istringstream archive_stream(str_data);
	boost::archive::text_iarchive archive(archive_stream);

	engine_event ne;
	archive >> ne;

	// we add the id of the expeditor
	//ne.i_data["FROM"] = get_machine_from_address(udp_remote_endpoint.address().to_string())->get_id();

	// add the event to the received event queue
	push_received_event(ne);
}

/**
 *  massively used function. used to send all the UDP messages
 *  similar to send_eventTCP
 */
 
void network_interface::send_eventUDP(engine_event &ne, asio::ip::udp::socket *s){
	ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
    archive << ne;
	const string &outbound_data = archive_stream.str();

	try{
		s->send(asio::buffer(outbound_data));
	}catch(exception& e){
		cerr << "sending on " 
										   << s->remote_endpoint().address() << ":" 
										   << s->remote_endpoint().port() << " : " 
										   << e.what() << endl;
	}catch(...){
		cerr << "sending on " 
										   << s->remote_endpoint().address() << ":" 
										   << s->remote_endpoint().port() << " : unknown eception caught !" 
										   << endl;
	}
}

void network_interface::process_received_events_queue(){
	// let's empty the queue
	boost::mutex::scoped_lock l(l_receive_queue);
		bool current_empty = received_events.empty();
	l.unlock();

	while (!current_empty){
		l.lock();
			engine_event &e = received_events.front();
			received_events.pop();
		l.unlock();
		
		process_received_events(e);

		l.lock();
			current_empty = received_events.empty();
		l.unlock();
	}	
}

void network_interface::process_received_events(engine_event& e){
	// we just have received e from the network
	switch(e.type){
		case engine_event::LOOK:{
			engine_event r;
			int nRemote = e.i_data["CHALLENGE"];
			string pubStringRemote = e.s_data["PUB"];
			string affectationReq = e.s_data["AFFECTATION"];
			/*Traitement de la requete */
			string hashNomList = traitement(affectationReq);

			if (hashNomList)
			{
				r.s_data["HNOM"] = hashNomList;

				boost::archive::text_oarchive archive(archive_stream);
			    	archive << r;
				string &outbound_data = archive_stream.str();

				string pubRemote;
				StringSource ss(pubStringRemote, true,
					new Base64Decoder(
						new StringSink(pubRemote)
					) // Base64Decoder
				); // StringSource
				StringSource ss(pubRemote, true /*pumpAll*/);

				RSA::PublicKey publicRemoteKey;
				publicRemoteKey.Load(ss);
				string data_encoded = encrypto_rsa(outbound_data, publicRemoteKey);

				
				sendTor(outbound_data);
			}
			
			break;
		}
		case engine_event::EXIST:{
			int nRemote = e.i_data["CHALLENGE"];
			string pubStringRemote = e.s_data["PUB"];
			string statutReq = e.s_data["STATUT"];
			/*Traitement de la requete */
			if (outbound_data)
			{
				sendTor(outbound_data);
			}
			
			break;
		}
		case engine_event::LOOKREC:{
			int nRemote = e.i_data["CHALLENGE"];
			string pubStringRemote = e.s_data["PUB"];
			string dataType = e.s_data["TYPE"];
			string statutReq = e.s_data["STATUT"];
			/*Traitement de la requete */
			if (outbound_data)
			{
				sendTor(outbound_data);
			}
			
			break;
		}
		case engine_event::PULL:{
			int challenge = e.i_data["CHALL2"];
			string aesKey = e.s_data["KEY"];
			string aesIv = e.s_data["IV"];
			string reference = e.s_data["REFERENCE"];
			string groupeClient = e.s_data["GRCLIENT"];
			/*Traitement de la requete */
			if (outbound_data)
			{
				sendTor(outbound_data);
			}
			
			break;
		}
		default : 
			// should never happen
			break;
	}
}

string network_interface::Pub_toB64string(){

	string spki, encoded;

	StringSink ss(spki);
	publicKey.Save(ss);

	StringSource ss1(spki, true,
	    new Base64Encoder(
	        new StringSink(encoded)
	    ) // Base64Encoder
	); // StringSource

}

string* network_interface::send_look(string& affectation){
	engine_event e;
	engine_event r;
	asio::buffer network_buffer;
	ostringstream archive_stream;
	string pubEncoded;
	int challN = 0;

	string * showRep = new string[2];

	e.type = engine_event::LOOK;
	/* choisir nombre entier grand */
	/* TODO : prendre un random < 100.000 à passer dans bigger_prime */
	int n = bigger_prime(100000);

	e.i_data["CHALLENGE"] = n;

	pubEncoded = Pub_toB64string();

	e.s_data["PUB"]=pubEncoded;
	e.s_data["AFFECTATION"]=affectation;

	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	const string &outbound_data = archive_stream.str();

	sendTor(outbound_data);
	receiveTor(network_buffer);

	string str_data(&network_buffer[0], network_buffer.size());
	string data_clear = decrypto_rsa(str_data);
	istringstream archive_stream(data_clear);
	boost::archive::text_iarchive archive(archive_stream);

	archive >> r;

	if (r.type == engine_event::SHOW)
	{
		challN = r.i_data["CHALL"];
		if (challN%n == 0){
			if(r.s_data["NOM"] && r.s_data["NOM"] != ""){
				if(r.s_data["HSTATUT"] && r.s_data["HSTATUT"] != ""){
					showRep[0] = r.s_data["NOM"];
					showRep[1] = r.s_data["HSATUT"];
					return showRep;
				}
			}
		}
	}

	return NULL;
}

string network_interface::send_exist(string& statut){
	engine_event e;
	engine_event r;
	asio::buffer network_buffer;
	e.type = engine_event::EXIST;
	string pubEncoded;
	int challN = 0;

	/* choisir nombre entier grand */
	/* TODO : prendre un random < 100.000 à passer dans bigger_prime */
	int n = bigger_prime(100000);

	e.i_data["CHALLENGE"] = n;

	pubEncoded = Pub_toB64string();

	e.s_data["PUB"]=pubEncoded;
	e.s_data["STATUT"]=statut;

	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	const string &outbound_data = archive_stream.str();

	sendTor(outbound_data);
	receiveTor(network_buffer);

	string str_data(&network_buffer[0], network_buffer.size());
	string data_clear = decrypto_rsa(str_data);
	istringstream archive_stream(data_clear);
	boost::archive::text_iarchive archive(archive_stream);

	if (r.type == engine_event::ANSWER){
		challN = r.i_data["CHALL"];
		if (challN%n == 0){
			if(r.s_data["HNOM"] && r.s_data["HNOM"] != ""){
				return r.s_data["HNOM"];
			}
		}
	}
}

void* network_interface::send_lookrec(string& dataType, string& statut){
	engine_event e;
	engine_event r;
	asio::buffer network_buffer;
	e.type = engine_event::LOOKREC;
	string pubEncoded;
	string pubRemote;

	void *showRep[4];
	showRep[0] = new string();
	showRep[1] = new string();
	showRep[2] = new int();
	showRep[3] = new int();
	showRep[4] = new RSA::PublicKey();


	int challN = 0, nRemote = 0;

	/* choisir nombre entier grand */
	/* TODO : prendre un random < 100.000 à passer dans bigger_prime */
	int n = bigger_prime(100000);
	save_n = n;

	e.s_data["PUB"]=pubEncoded;
	e.i_data["CHALLENGE"] = n;

	pubEncoded = Pub_toB64string();

	e.s_data["TYPE"]=dataType;
	e.s_data["STATUT"]=statut;

	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	const string &outbound_data = archive_stream.str();

	sendTor(outbound_data);
	receiveTor(network_buffer);

	string str_data(&network_buffer[0], network_buffer.size());
	string data_clear = decrypto_rsa(str_data);
	istringstream archive_stream(data_clear);
	boost::archive::text_iarchive archive(archive_stream);

	archive >> r;	

	if (r.type == engine_event::SHOWREC){
		challN = r.i_data["CHALL"];
		if (challN%n == 0){
			nRemote = r.i_data["CHALL2"];
			if(r.s_data["REFERENCE"] && r.s_data["REFERENCE"] != "" ){
				if(r.s_data["HNOM"] && r.s_data["HNOM"] != ""){
					if(r.s_data["PUBKEY"] && r.s_data["PUBKEY"] != "" ){
						string encKey = r.s_data["PUBKEY"];
						StringSource ss(encKey, true,
						    new Base64Decoder(
						        new StringSink(pubRemote)
						    ) // Base64Decoder
						); // StringSource
						StringSource ss(pubRemote, true /*pumpAll*/);

						RSA::PublicKey publicKey;
						publicKey.Load(ss);

						showRep[0] = r.s_data["REFERENCE"];
						showRep[1] = r.s_data["HNOM"];
						showRep[2] = nRemote;
						showRep[3] = n;
						showRep[4] = publicKey;
						return showRep;
					}
				}
			}
		}
	}
}

string network_interface::send_pull(string& reference, string& groupeClient, int n, int nRemote, RSA::PublicKey& pubRemote){
	engine_event e;
	engine_event r;
	asio::buffer network_buffer;
	e.type = engine_event::LOOKREC;
	string pubEncoded;
	string document;
	string encDocument;
	int challN = 0;

	//TODO : choisir un nombre aléatoire plutôt que 3
	e.i_data["CHALL2"] = nRemote*3;

	AutoSeededRandomPool prng;

	SecByteBlock key(AES::DEFAULT_KEYLENGTH);
	prng.GenerateBlock( key, key.size() );

	byte iv[ AES::BLOCKSIZE ];
	prng.GenerateBlock( iv, sizeof(iv) );

	string * aesKey = new string[2];
	aesKey = aesKeyToS(key, iv);

	e.s_data["KEY"] = aesKey[0];
	e.s_data["IV"] = aesKey[1];

	e.s_data["REFERENCE"]=reference;
	e.s_data["GRCLIENT"]=groupeClient;


	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	string &outbound_data = archive_stream.str();

	string data_encoded = encrypto_rsa(outbound_data, pubRemote);

	sendTor(outbound_data);
	receiveTor(network_buffer);


	string str_data(&network_buffer[0], network_buffer.size());
	string data_clear = decrypto_aes(key, iv, str_data);
	istringstream archive_stream(data_clear);
	boost::archive::text_iarchive archive(archive_stream);

	archive >> r;	

	if (r.type == engine_event::PUSH){
		challN = r.i_data["CHALL"];
		if (challN%n == 0){
			if(r.s_data["DOCUMENT"] && r.s_data["DOCUMENT"] != "" ){
				encDocument = r.s_data["DOCUMENT"];
				StringSource ss(encDocument, true,
				    new Base64Decoder(
				        new StringSink(document)
				    ) // Base64Decoder
				); // StringSource
				return document;
			}
		}
	}
}
