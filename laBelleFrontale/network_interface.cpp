#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <vector>
#include <iostream>

#include <QCoreApplication>

#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>

#include "clientFront.h"
#include "network_interface.h"
#include "engine_event.h"
#include "utils.h"
#include "traitement.h"
#include "message.h"


using namespace std;
using namespace CryptoPP;

/**
 * constructeur
 */
network_interface::network_interface(bdd_tcp * outbdd, Tslot * ourts, Tslot * ourts_s , reception * outres): bdd(outbdd), ts(ourts), ts_s(ourts_s), res(outres) {
	running = true;
	host_rem = "127.0.0.1";
    	port_rem = 8082;

    	recBool = false;
    	responseRec = "";

	///////////////////////////////////////
	// Generate Parameters
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);

	///////////////////////////////////////
	// Create Keys
	privateKey = CryptoPP::RSA::PrivateKey(params);
	publicKey = CryptoPP::RSA::PublicKey(params);

	// With the current version of Crypto++, MessageEnd() needs to be called
	// explicitly because Base64Encoder doesn't flush its buffer on destruction.
	Base64Encoder privkeysink(new FileSink("myprivkey_net.txt"));
	privateKey.DEREncode(privkeysink);
	privkeysink.MessageEnd();
	
	Base64Encoder pubkeysink(new FileSink("mypubkey_net.txt"));
	publicKey.DEREncode(pubkeysink);
	pubkeysink.MessageEnd();
	
	
	// en commentaire en attendant le module reseau
	/*try{
		get_config_data();
	}catch(exception&e){
		cerr << e.what() << endl;
	}catch(string& e){
		// cannot create network, will continue without networking, no listening, no connect
		cerr << e << endl;
		running = false;
	}catch(...){
		cerr << "unknown exception handled" << endl;
	}*/

}

void network_interface::spawn(){
	for (; port_rem < 8082+30 ; ++port_rem){

		try
		{
		// Création d'un NoeudThor

		boost::asio::io_service io_service;
		cout << "Noeud mis en place sur le port " << port_rem << endl;
		noeudthor = new NoeudThor(io_service, port_rem, "127.0.0.1", 8080, "127.0.0.1", 8081, this);
		io_service.run();

		break;
		}
		catch (std::exception& e)
		{
			if (string("bind: Address already in use") == e.what() || string("bind: Adresse déjà utilisée") == e.what()){
				cerr << e.what() << endl;
			}
			else {
				cout << "Exit with error" << endl;
				cerr << e.what() << endl;
				exit(-1);
			}
		}
	}
}


network_interface::~network_interface(){	
	// en commentaire en attendant le module reseau	
	//boost::mutex::scoped_lock l(state_mutex);
	//	state = STATE_QUITTING;
	//l.unlock();

	//io.stop();
}

bool network_interface::getRecbool(){

	return recBool;
}

void network_interface::setRecbool(bool reBool){

	recBool = reBool;
}

string network_interface::getResp(){

	return responseRec;
}

void network_interface::setResp(string reResp){

	responseRec = reResp;
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

string network_interface::encrypto_rsa(string& plain, CryptoPP::RSA::PublicKey pubRemote){

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

	try
	{
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
	catch( const CryptoPP::Exception& e )
	{
		cerr << e.what() << endl;
		cout << "Ce paquet n'est pas pour moi." << endl;
		return "";
	}
	

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
string network_interface::encrypto_aes(SecByteBlock key, byte* iv, string& plain){

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

	return aesString;
}

byte* network_interface::sToB(string plain){

	string clear;

	StringSource ss5( plain , true, 
	        new HexDecoder(
	            new StringSink( clear )
	        ) // StreamTransformationFilter      
	); // StringSource

	byte * conv = new byte(clear.size()+1);
	conv = reinterpret_cast<unsigned char*>(const_cast<char*>(clear.data()));


	return conv;
}

SecByteBlock network_interface::sToSbb(string plain){

	string clear;

	StringSource ss5( plain , true, 
	        new HexDecoder(
	            new StringSink( clear )
	        ) // StreamTransformationFilter      
	); // StringSource

	SecByteBlock conv(reinterpret_cast<const unsigned char*>(clear.data()), clear.size());

	return conv;
}
/*
void network_interface::get_config_data(){	
	// non utilisé en attendant le module reseau
	port_udp_reception = 4447;
	
	// starts the network and set up all the async_callbacks
	// TODO : regarder configuration broadcast
	boost::asio::ip::udp::endpoint udp_listening_ep(boost::asio::ip::udp::v4(), port_udp_reception);
	s_udp_in = new boost::asio::ip::udp::socket(io, udp_listening_ep);

	try{

	s_udp_in->async_receive_from(	boost::asio::buffer(network_buffer, BUFFER_SIZE), 
									udp_remote_endpoint, 
									boost::bind(&network_interface::UDP_async_read, 
												this, 
												boost::asio::placeholders::error,
												boost::asio::placeholders::bytes_transferred)
								);
	}catch(exception&e){
		cerr << e.what() << endl;
	}catch(...){
		cerr << "unknown exception handled" << endl;
	}

	cout << "UDP listening " << s_udp_in->local_endpoint().address() << ":" << s_udp_in->local_endpoint().port()  << endl;

	boost::asio::detail::thread(boost::bind(&boost::asio::io_service::run, &io));
}*/

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
void network_interface::tor_receive(string str_data){

	// non utilisé en attendant le module reseau

	// check if the messages comes from a registered machine	
	/*if (!is_address_registered(udp_remote_endpoint.address().to_string()))
		return;*/
	
	// let's deserialize the message


	//string str_data(&network_buffer[0], network_buffer.size());

	char buffer[26];
	size_t length = str_data.copy(buffer,25,0);
	buffer[length]='\0';
	string testHeader = string(buffer, length);
	engine_event ne;

	cout << "on a recu ca : " << str_data << endl;


	if (testHeader.compare("22 serialization::archive") == 0){
		
		istringstream archive_stream(str_data);
		boost::archive::text_iarchive archive(archive_stream);
		
		archive >> ne;
		process_received_events(ne);
		return;
	}
	else{
		string data_clear ;
		
		if(!(data_clear = decrypto_rsa(str_data)).empty()){

			istringstream archive_stream(data_clear);
			boost::archive::text_iarchive archive(archive_stream);

			archive >> ne;
			process_received_events(ne);
			return;

		}
		else{
		
			cout << "J'ai recu un paquet qui n'était pas pour moi" << endl;
			return;

		}

	}

	cout << "J'ai recu un paquet qui n'était pas pour moi" << endl;


	// we add the id of the expeditor
	//ne.i_data["FROM"] = get_machine_from_address(udp_remote_endpoint.address().to_string())->get_id();

	// add the event to the received event queue
	//push_received_event(ne);
}

/**
 *  massively used function. used to send all the UDP messages
 *  similar to send_eventTCP
 */
 /*
void network_interface::send_eventUDP(engine_event &ne, boost::asio::ip::udp::socket *s){
	// non utilisé  en attendant le module reseau
	ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
    	archive << ne;
	const string &outbound_data = archive_stream.str();

	try{
		s->send(boost::asio::buffer(outbound_data));
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
}*/

void network_interface::push_received_event(engine_event& e){
	boost::mutex::scoped_lock l(l_receive_queue);
		received_events.push(e);
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

string network_interface::treat_resource(string& request, string& token, int action, string requetebdd){

	const unsigned char key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88}; 

	list<string> * a_traiter;
	list<string> a_traiter_clair;

	QString qreqFormat = QString("%1").arg(request.data());

	Message msg2(qreqFormat,'2','*');
	//msg2.chiffrement(key);
	string chiffrer = msg2.crypt((unsigned char*) request.data(), request.size());

	//string toto = msg2.getChiffre().toStdString();

	/* --- On interroge le client --- */
	clientFront cli;

	cli.socBind();
	cli.emission(chiffrer);

	if(ts_s->TriggerToken(token) == false) return "";

	srand (time(NULL));
	int ms = rand() % 200 + 2000;

	a_traiter = ts_s->startTimer(token,ms);


	if (!a_traiter->empty())
	{
		for (auto it : *a_traiter) {

			string messageStr = it;
			QString message = QString("%1").arg(messageStr.data());
			Message msg(message,'*');
			//msg.dechiffrement(key);

			//string rep = msg.getMsg().toStdString();
			string rep = msg.decrypt((unsigned char*) messageStr.data(), messageStr.size());

			a_traiter_clair.push_back(rep);

		}
	}
	/* ------------------------------------- */

	if(type > 2){

		QString versBdd = QString("%1").arg(requetebdd.c_str());

	            bdd->emission(versBdd);
	                
	            bdd->attendLecture(200);

	            string clair = bdd->getMsg().toStdString();

	            if(!clair.empty())
	            	a_traiter_clair.push_back(clair);

	}


	string repFormat = traitement_rep_client(a_traiter_clair);

	return repFormat;
}

void network_interface::process_received_events(engine_event& e){

	// we just have received e from the network

	//const unsigned char key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88}; 

	ostringstream archive_stream;
	ostringstream archive_streamOut;
	ostringstream archive_streamPckt;
	switch(e.type){
		case engine_event::LOOK:{

		 	engine_event r;
			engine_event p;
			vector<string> groups;

			string reqFormat;

			string pubStringRemote = e.s_data["PUB"];
			string affectationReq = e.s_data["AFFECTATION"];
			string token = e.s_data["TOKEN"];

			groups.push_back("none");
			reqFormat = traitement_req_client(token,"1", "none", affectationReq, groups,"none","none", "none");

			string repFormat = treat_resource(reqFormat, token);

			if (!repFormat.empty() || repFormat != "")
			{
				r.type = engine_event::SHOW;
				r.s_data["REPONSE"] = repFormat;
				r.s_data["TOKEN"] = token;

				cout << "repFormat : " << repFormat << endl;

				boost::archive::text_oarchive archive(archive_stream);
				archive << r;
				string outbound_data = archive_stream.str();

				string pubRemote;
				StringSource ss(pubStringRemote, true,
					new Base64Decoder(
						new StringSink(pubRemote)
					) // Base64Decoder
				); // StringSource
				StringSource ss2(pubRemote, true /*pumpAll*/);

				CryptoPP::RSA::PublicKey publicRemoteKey;
				publicRemoteKey.Load(ss2);
				AutoSeededRandomPool prng;

				SecByteBlock key(AES::DEFAULT_KEYLENGTH);
				prng.GenerateBlock( key, key.size() );

				byte iv[ AES::BLOCKSIZE ];
				prng.GenerateBlock( iv, sizeof(iv) );

				string cipher_data = encrypto_aes(key, iv, outbound_data);

				p.type = engine_event::SECRET;
				p.s_data["CIPHER"] = cipher_data;

				string * aesKey = new string[2];
				aesKey = aesKeyToS(key, iv);

				string aesKey_encoded = encrypto_rsa(aesKey[0], publicRemoteKey);
				string aesIv_encoded = encrypto_rsa(aesKey[1], publicRemoteKey);

				p.s_data["KEY"] = aesKey_encoded;
				p.s_data["IV"] = aesIv_encoded;

				boost::archive::text_oarchive archiveOut(archive_streamOut);
				archiveOut << p;
				const string &data_encoded = archive_streamOut.str();

				noeudthor->send(data_encoded);

			}

			break;
		}
		case engine_event::EXIST:{
			engine_event r;
			engine_event p;
			string reqFormat;
			vector<string> groups;

			string pubStringRemote = e.s_data["PUB"];
			string affectationReq = e.s_data["AFFECTATION"];
			string token = e.s_data["TOKEN"];
			/*Traitement de la requete */

			groups.push_back("none");
			reqFormat = traitement_req_client(token,"2", "none", affectationReq, groups,"none","none", "none");

			string repFormat = treat_resource(reqFormat, token);

			if (!repFormat.empty() || repFormat != "")
			{
			       r.type = engine_event::ANSWER;
			       r.s_data["HASH"] = repFormat;
			       r.s_data["TOKEN"] = token;

			       boost::archive::text_oarchive archive(archive_stream);
			       archive << r;
			       string outbound_data = archive_stream.str();

			       string pubRemote;
			       StringSource ss(pubStringRemote, true,
				new Base64Decoder(
					new StringSink(pubRemote)
				) // Base64Decoder
			        ); // StringSource
			        StringSource ss2(pubRemote, true /*pumpAll*/);

			        CryptoPP::RSA::PublicKey publicRemoteKey;
			        publicRemoteKey.Load(ss2);
			        AutoSeededRandomPool prng;

			        SecByteBlock key(AES::DEFAULT_KEYLENGTH);
			        prng.GenerateBlock( key, key.size() );

			        byte iv[ AES::BLOCKSIZE ];
			        prng.GenerateBlock( iv, sizeof(iv) );

			        string cipher_data = encrypto_aes(key, iv, outbound_data);

			        p.type = engine_event::SECRET;
			        p.s_data["CIPHER"] = cipher_data;

			        string * aesKey = new string[2];
			        aesKey = aesKeyToS(key, iv);

			        string aesKey_encoded = encrypto_rsa(aesKey[0], publicRemoteKey);
			        string aesIv_encoded = encrypto_rsa(aesKey[1], publicRemoteKey);

			        p.s_data["KEY"] = aesKey_encoded;
			        p.s_data["IV"] = aesIv_encoded;

			        boost::archive::text_oarchive archiveOut(archive_streamOut);
			        archiveOut << p;
			        const string &data_encoded = archive_streamOut.str();

			        noeudthor->send(data_encoded);

			}
			
			break;
		}
		case engine_event::LOOKREC:{

			engine_event r;
			engine_event p;
			vector<string> groups;

			string reqFormat;
			string reqFormatBdd;
			string pubStringRemote = e.s_data["PUB"];
			string dataType = e.s_data["TYPE"];
			string affectationReq = e.s_data["AFFECTATION"];
			string token = e.s_data["TOKEN"];

			groups.push_back("none");
			reqFormat = traitement_req_client(token,"3", "none", affectationReq, groups,dataType,"none", "none");
			reqFormatBdd = traitement_req_bdd(token,"300", "none", affectationReq, groups,dataType,"none", "none");

			string repFormat = treat_resource(reqFormat, token, 3, reqFormatBdd);

			if (!repFormat.empty() || repFormat != "")
			{
				r.type = engine_event::SHOWREC;

				r.s_data["REPONSE"] = repFormat;
				r.s_data["TOKEN"] = token;

				string pubEncoded = Pub_toB64string();
				r.s_data["PUBKEY"] = pubEncoded;

				boost::archive::text_oarchive archive(archive_stream);
			    	archive << r;
				string outbound_data = archive_stream.str();

				string pubRemote;
				StringSource ss(pubStringRemote, true,
					new Base64Decoder(
						new StringSink(pubRemote)
					) // Base64Decoder
				); // StringSource
				StringSource ss2(pubRemote, true /*pumpAll*/);

				CryptoPP::RSA::PublicKey publicRemoteKey;
				publicRemoteKey.Load(ss2);

			           AutoSeededRandomPool prng;

			           SecByteBlock key(AES::DEFAULT_KEYLENGTH);
			           prng.GenerateBlock( key, key.size() );

			           byte iv[ AES::BLOCKSIZE ];
			           prng.GenerateBlock( iv, sizeof(iv) );

			           string cipher_data = encrypto_aes(key, iv, outbound_data);

			           p.type = engine_event::SECRET;
			           p.s_data["CIPHER"] = cipher_data;

			           string * aesKey = new string[2];
			           aesKey = aesKeyToS(key, iv);

			           string aesKey_encoded = encrypto_rsa(aesKey[0], publicRemoteKey);
			           string aesIv_encoded = encrypto_rsa(aesKey[1], publicRemoteKey);

			           p.s_data["KEY"] = aesKey_encoded;
			           p.s_data["IV"] = aesIv_encoded;

			           boost::archive::text_oarchive archiveOut(archive_streamOut);
			           archiveOut << p;
			           const string &data_encoded = archive_streamOut.str();

			           noeudthor->send(data_encoded);

			}
			
			break;
		}
		case engine_event::SECRET:{
			engine_event r;
			
			string showRep = "";

			byte * iv;
			SecByteBlock key;

			bool stahp = false;

			if(e.s_data["KEY"].compare("N/A") == 0 && e.s_data["IV"].compare("N/A") == 0){

				/* reponse push */
				if(!aesIvTampon.empty() && !aesKeyTampon.empty() && aesIvTampon != "" && aesKeyTampon != ""){
					iv = sToB(aesIvTampon);
					key = sToSbb(aesKeyTampon);
				}
				else {
					iv = sToB("");
					key = sToSbb("");

					stahp = true;
					
				}
			}
			else{
				string aesKey = decrypto_rsa(e.s_data["KEY"]);
				string aesIv = decrypto_rsa(e.s_data["IV"]);

				iv = sToB(aesIv);
				key = sToSbb(aesKey);
				
			}
			if(stahp) break;

			string data_clear = decrypto_aes(key, iv, e.s_data["CIPHER"]);
			
			data_clear.erase(0, 16);
			data_clear = "22 serialization" + data_clear;
			cout << data_clear << endl;

			istringstream archive_streamPckt(data_clear);

			boost::archive::text_iarchive archivePckt(archive_streamPckt);

			archivePckt >> r;

			switch(r.type){
				case engine_event::SHOW:{

					if(!r.s_data["REPONSE"].empty() && r.s_data["REPONSE"] != ""){
	
						//showRep[0] = r.s_data["NOM"];
						//showRep[1] = r.s_data["HSTATUT"];
						//responseRec = 
						string repShow = r.s_data["REPONSE"];
						string token = r.s_data["TOKEN"];

						boost::mutex * mustStopMutex = ts->getMutex(token);
						mustStopMutex->lock();
						bool mustStop = ts->getBool(token);
						cout << "repshow : " << repShow << endl;
						if (mustStop == false)
							ts->addMessageToList(token, repShow);
						mustStopMutex->unlock();
						
					}
				}
				case engine_event::ANSWER:{
			
					if(!r.s_data["HASH"].empty() && r.s_data["HASH"] != ""){
						string hash = r.s_data["HASH"];
						string token = r.s_data["TOKEN"];
						//hnom.erase(hnom.size() - 1, 1);
						//cout << "hash : " << hash << endl;
						//responseRec = hash;	
						//recBool = true;

						boost::mutex * mustStopMutex = ts->getMutex(token);
						mustStopMutex->lock();
						bool mustStop = ts->getBool(token);
						if (mustStop == false)
							ts->addMessageToList(token, hash);
						mustStopMutex->unlock();
					
					}
				}
				case engine_event::SHOWREC:{

					if(!r.s_data["REPONSE"].empty() && r.s_data["REPONSE"] != ""){
						if(!r.s_data["PUBKEY"].empty() && r.s_data["PUBKEY"] != "" ){
							string encKey = r.s_data["PUBKEY"];
							string token = r.s_data["TOKEN"];
							string repShowrec = r.s_data["REPONSE"];
							string pubRemote;
							StringSource ss(encKey, true,
							    new Base64Decoder(
							        new StringSink(pubRemote)
							    ) // Base64Decoder
							); // StringSource
							StringSource ss2(pubRemote, true /*pumpAll*/);

							CryptoPP::RSA::PublicKey publicKeyRemote;
							publicKeyRemote.Load(ss2);

							repShowrec += "*" + Pub_toB64string(publicKeyRemote);

							cout << "RepShowRec : %s" << repShowrec << endl;

							boost::mutex * mustStopMutex = ts->getMutex(token);
							mustStopMutex->lock();
							bool mustStop = ts->getBool(token);
							if (mustStop == false)
								ts->addMessageToList(token, repShowrec);
							mustStopMutex->unlock();
							
							
						}
					}

				}
				case engine_event::PUSH:{
					if(!r.s_data["DOCUMENT"].empty() && r.s_data["DOCUMENT"] != ""){
						string encDocument = r.s_data["DOCUMENT"];
						string token = r.s_data["TOKEN"];
						string document;

						StringSource ss(encDocument, true,
						    new Base64Decoder(
						        new StringSink(document)
						    ) // Base64Decoder
						); // StringSource
						//hnom.erase(hnom.size() - 1, 1);

						cout << "document : " << document << endl;

						boost::mutex * mustStopMutex = ts->getMutex(token);
						mustStopMutex->lock();
						bool mustStop = ts->getBool(token);
						if (mustStop == false)
							ts->addMessageToList(token, document);
						mustStopMutex->unlock();
					
					}
				}
				case engine_event::PULL:{
					engine_event p;
					engine_event q;

					string aesKey = r.s_data["KEY"];
					string aesIv = r.s_data["IV"];
					string reference = r.s_data["REFERENCE"];
					string groClient = r.s_data["GRCLIENT"];
					string token = r.s_data["TOKEN"];
					vector<string> groupeClient;

					string reqFormat;
					string reqFormatBdd;

					istringstream issGroupe(groClient);
					string groupeUnique;
					while ( std::getline( issGroupe, groupeUnique, '*' )) 
					{ 
						groupeClient.push_back(groupeUnique);
					}

					/*Traitement de la requete */
					
					reqFormat = traitement_req_client(token,"4", "none", "none", groupeClient,"none",reference, "none");
					reqFormatBdd = traitement_req_bdd(token,"301", "none", "none", groupeClient,"none",reference, "none");

					string document = treat_resource(reqFormat, token, 4, reqFormatBdd);			

					if (!document.empty() || document != "")
					{
						p.type = engine_event::PUSH;
						p.s_data["DOCUMENT"] = document;
						r.s_data["TOKEN"] = token;

						boost::archive::text_oarchive archive(archive_stream);
						archive << p;
						string outbound_data = archive_stream.str();

						byte * iv = sToB(aesIv);
						SecByteBlock key = sToSbb(aesKey);

						string cipher_data = encrypto_aes(key, iv, outbound_data);

					            q.type = engine_event::SECRET;
					            q.s_data["CIPHER"] = cipher_data;
					            q.s_data["KEY"] = "N/A";
					            q.s_data["IV"] = "N/A";

					            boost::archive::text_oarchive archiveOut(archive_streamOut);
					            archiveOut << q;
					            const string &data_encoded = archive_streamOut.str();

						noeudthor->send(data_encoded);
						
					}
					
					break;
				}
			}	
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

	return encoded;

}

string network_interface::Pub_toB64string(CryptoPP::RSA::PublicKey publicRemoteKey){

	string spki, encoded;

	StringSink ss(spki);
	publicRemoteKey.Save(ss);

	StringSource ss1(spki, true,
	    new Base64Encoder(
	        new StringSink(encoded)
	    ) // Base64Encoder
	); // StringSource

	return encoded;

}

void network_interface::send_look(string& affectation, string& token){

	///////////////////////////////////////
	// Generate Parameters
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);

	///////////////////////////////////////
	// Create Keys
	privateKey = CryptoPP::RSA::PrivateKey(params);
	publicKey = CryptoPP::RSA::PublicKey(params);

	engine_event e;
	//boost::asio::buffer network_buffer;
	ostringstream archive_stream;
	string pubEncoded;


	e.type = engine_event::LOOK;

	pubEncoded = Pub_toB64string();
	
	e.s_data["PUB"]=pubEncoded;
	e.s_data["AFFECTATION"]=affectation;
	e.s_data["TOKEN"]=token;

	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	const string &outbound_data = archive_stream.str();

	noeudthor->send(outbound_data);

}

void network_interface::send_exist(string& affectation, string& token){


	///////////////////////////////////////
	// Generate Parameters
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);

	///////////////////////////////////////
	// Create Keys
	privateKey = CryptoPP::RSA::PrivateKey(params);
	publicKey = CryptoPP::RSA::PublicKey(params);

	engine_event e;
	//boost::asio::buffer network_buffer;
	e.type = engine_event::EXIST;
	ostringstream archive_stream;
	string pubEncoded;

	pubEncoded = Pub_toB64string();

	e.s_data["PUB"]=pubEncoded;
	e.s_data["AFFECTATION"]=affectation;
	e.s_data["TOKEN"]=token;

	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	const string &outbound_data = archive_stream.str();

	noeudthor->send(outbound_data);

}

void network_interface::send_lookrec(string& dataType, string& affectation, string& token){

	///////////////////////////////////////
	// Generate Parameters
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);

	///////////////////////////////////////
	// Create Keys
	privateKey = CryptoPP::RSA::PrivateKey(params);
	publicKey = CryptoPP::RSA::PublicKey(params);

	engine_event e;

	e.type = engine_event::LOOKREC;
	ostringstream archive_stream;
	string pubEncoded;


	pubEncoded = Pub_toB64string();

	e.s_data["PUB"]=pubEncoded;

	e.s_data["TYPE"]=dataType;
	e.s_data["AFFECTATION"]=affectation;
	e.s_data["TOKEN"]=token;

	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	const string &outbound_data = archive_stream.str();

	//sendTor(outbound_data);
	//receiveTor(network_buffer);
	noeudthor->send(outbound_data);

}

void network_interface::send_pull(string& reference, string& groupeClient, string& encKey, string& token){
	engine_event e;
	engine_event p;

	e.type = engine_event::PULL;
	ostringstream archive_stream;
	ostringstream archive_streamOut;
	string pubEncoded;
	string document;
	string encDocument;
	string pubRemote;

	StringSource ss(encKey, true,
	    new Base64Decoder(
	        new StringSink(pubRemote)
	    ) // Base64Decoder
	); // StringSource
	StringSource ss2(pubRemote, true /*pumpAll*/);

	CryptoPP::RSA::PublicKey publicKeyRemote;
	publicKeyRemote.Load(ss2);

	AutoSeededRandomPool prng;

	SecByteBlock key(AES::DEFAULT_KEYLENGTH);
	prng.GenerateBlock( key, key.size() );

	byte iv[ AES::BLOCKSIZE ];
	prng.GenerateBlock( iv, sizeof(iv) );

	string * aesKey_1 = new string[2];
	aesKey_1 = aesKeyToS(key, iv);

	e.s_data["KEY"] = aesKey_1[0];
	e.s_data["IV"] = aesKey_1[1];

	e.s_data["REFERENCE"]=reference;
	e.s_data["GRCLIENT"]=groupeClient;
	e.s_data["TOKEN"]=token;

	aesKeyTampon = aesKey_1[0];
	aesIvTampon = aesKey_1[1];

	boost::archive::text_oarchive archive(archive_stream);
    	archive << e;
	string outbound_data = archive_stream.str();

           AutoSeededRandomPool prng2;

           SecByteBlock key_p(AES::DEFAULT_KEYLENGTH);
           prng2.GenerateBlock( key_p, key_p.size() );

           byte iv_p[ AES::BLOCKSIZE ];
           prng2.GenerateBlock( iv_p, sizeof(iv_p) );

           string cipher_data = encrypto_aes(key_p, iv_p, outbound_data);

           p.type = engine_event::SECRET;
           p.s_data["CIPHER"] = cipher_data;

           string * aesKey = new string[2];
           aesKey = aesKeyToS(key_p, iv_p);

           string aesKey_encoded = encrypto_rsa(aesKey[0], publicKeyRemote);
           string aesIv_encoded = encrypto_rsa(aesKey[1], publicKeyRemote);

           p.s_data["KEY"] = aesKey_encoded;
           p.s_data["IV"] = aesIv_encoded;

           boost::archive::text_oarchive archiveOut(archive_streamOut);
           archiveOut << p;
           const string &data_encoded = archive_streamOut.str();

	noeudthor->send(data_encoded);

}
