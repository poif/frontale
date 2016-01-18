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
#include "lecture.h"


using namespace std;
using namespace CryptoPP;


network_interface::network_interface(bdd_tcp * outbdd, Tslot * ourts, Tslot * ourts_s , reception * outres, Message *ourMsg): bdd(outbdd), ts(ourts), ts_s(ourts_s), res(outres) , mess(ourMsg) {
	running = true;
	host_rem = "127.0.0.1";
    	port_rem = 8082;

    	recBool = false;
    	responseRec = "";


	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);


	privateKey = CryptoPP::RSA::PrivateKey(params);
	publicKey = CryptoPP::RSA::PublicKey(params);


	Base64Encoder privkeysink(new FileSink("myprivkey_net.txt"));
	privateKey.DEREncode(privkeysink);
	privkeysink.MessageEnd();
	
	Base64Encoder pubkeysink(new FileSink("mypubkey_net.txt"));
	publicKey.DEREncode(pubkeysink);
	pubkeysink.MessageEnd();
	

}

void network_interface::spawn(){
	for (; port_rem < 8082+30 ; ++port_rem){

		try
		{
	

        lecture lec;

		boost::asio::io_service io_service;

        noeudthor = new NoeudThor(io_service, port_rem, lec.getServerCentrale() , 8080, lec.getServerProvider(), 8081, this);
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
		
		return "";
	}
	

}

string network_interface::encrypto_aes(SecByteBlock key, byte* iv, string& plain){

	string cipher;


	try
	{

	    CBC_Mode< AES >::Encryption e;
	    e.SetKeyWithIV( key, key.size(), iv );


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


void network_interface::frame(){

	process_received_events_queue();
}


void network_interface::tor_receive(string str_data){



	char buffer[26];
	size_t length = str_data.copy(buffer,25,0);
	buffer[length]='\0';
	string testHeader = string(buffer, length);
	engine_event ne;



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
		
			
			return;

		}

	}


}



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

	list<char* > * a_traiter;
	list<string> a_traiter_clair;

	list<int > * a_traiter_size;

	
	clientFront cli;

	for(int i=0; i < mess->getNbKey(); i++){
		
		char* chiffrer = mess->crypt((unsigned char*) request.data(), request.size(),i);

		cli.socBind();
		cli.emission(chiffrer, request.size()+16-(request.size()%16));
	}

	if(ts_s->TriggerToken(token) == false) return "";

	srand (time(NULL));
	int ms = rand() % 200 + 2000;

	a_traiter = ts_s->startTimer_c(token,ms);
	a_traiter_size = ts_s->getListSize(token);


	if (!a_traiter->empty())
	{
		list<int>::iterator it_size = a_traiter_size->begin();
		for (auto it : *a_traiter) {

			char * messageStr = it;
			int lasize = *it_size;


			string rep = mess->decrypt((unsigned char*) messageStr, lasize);

			a_traiter_clair.push_back(rep);
			it_size++;

		}
	}

	if(action > 2){

	            /*bdd->emission(requetebdd);
	                
	            bdd->attendLecture(200);

	            string clair = bdd->getMsg();

	            if(!clair.empty())
	            	a_traiter_clair.push_back(clair);*/

	}


	string repFormat = traitement_rep_client(a_traiter_clair);

	return repFormat;
}

void network_interface::process_received_events(engine_event& e){

	ostringstream archive_stream;
	ostringstream archive_streamOut;
	ostringstream archive_streamPckt;
	switch(e.type){
		case engine_event::LOOK:{

		 	engine_event r;
			engine_event p;

			string reqFormat;

			string pubStringRemote = e.s_data["PUB"];
			string affectationReq = e.s_data["AFFECTATION"];
			string token = e.s_data["TOKEN"];

			reqFormat = traitement_req_client(token,"1", "none", "none", affectationReq, "none", "none", "none", "none","none", "none");

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

				noeudthor->sendRep(data_encoded);

			}

			break;
		}
		case engine_event::EXIST:{
			engine_event r;
			engine_event p;
			string reqFormat;
			

			string pubStringRemote = e.s_data["PUB"];
			string affectationReq = e.s_data["AFFECTATION"];
			string token = e.s_data["TOKEN"];

			reqFormat = traitement_req_client(token,"2", "none", "none", affectationReq, "none", "none", "none","none","none", "none");

			string repFormat = treat_resource(reqFormat, token);
			cout << repFormat << endl;
			istringstream iss(repFormat);
			string temp;
			string temp2;
			getline(iss, temp, '*');
			getline(iss, temp2, '*');
			cout << "le hash : " << temp2 << endl;

			if (!repFormat.empty() || repFormat != "")
			{
			       r.type = engine_event::ANSWER;
			       r.s_data["HASH"] = temp2;
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

			        noeudthor->sendRep(data_encoded);

			}
			
			break;
		}
		case engine_event::LOOKREC:{

			engine_event r;
			engine_event p;

			string reqFormat;
			string reqFormatBdd;
			string pubStringRemote = e.s_data["PUB"];
			string dataType = e.s_data["TYPE"];
			string affectationReq = e.s_data["AFFECTATION"];
			string token = e.s_data["TOKEN"];

			reqFormat = traitement_req_client(token,"3", "none", "none", "none", affectationReq, "none", "none",dataType,"none", "none");
			reqFormatBdd = traitement_req_bdd(token,"300", "none", "none", "none", affectationReq, "none", "none",dataType,"none", "none");

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

			           noeudthor->sendRep(data_encoded);

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
	 
						string repShow = r.s_data["REPONSE"];
						string token = r.s_data["TOKEN"];

						boost::mutex * mustStopMutex = ts->getMutex(token);
						mustStopMutex->lock();
						bool mustStop = ts->getBool(token);

						if (mustStop == false)
							ts->addMessageToList(token, repShow);
						mustStopMutex->unlock();
						
					}
					break;
				}
				case engine_event::ANSWER:{
			
					if(!r.s_data["HASH"].empty() && r.s_data["HASH"] != ""){
						string hash = r.s_data["HASH"];
						string token = r.s_data["TOKEN"];
						hash = token + "*" + hash;

						boost::mutex * mustStopMutex = ts->getMutex(token);
						mustStopMutex->lock();
						bool mustStop = ts->getBool(token);
						if (mustStop == false)
							ts->addMessageToList(token, hash);
						mustStopMutex->unlock();
					
					}
					break;
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

							repShowrec += Pub_toB64string(publicKeyRemote);

							boost::mutex * mustStopMutex = ts->getMutex(token);
							mustStopMutex->lock();
							bool mustStop = ts->getBool(token);
							if (mustStop == false)
								ts->addMessageToList(token, repShowrec);
							mustStopMutex->unlock();
							
							
						}
					}
					break;

				}
				case engine_event::PUSH:{
					if(!r.s_data["DOCUMENT"].empty() && r.s_data["DOCUMENT"] != ""){
						string encDocument = r.s_data["DOCUMENT"];
						string token = r.s_data["TOKEN"];
						string document;


						document = encDocument;

						boost::mutex * mustStopMutex = ts->getMutex(token);
						mustStopMutex->lock();
						bool mustStop = ts->getBool(token);
						if (mustStop == false)
							ts->addMessageToList(token, document);
						mustStopMutex->unlock();
					
					}
					break;
				}
				case engine_event::PULL:{
					engine_event p;
					engine_event q;

					string aesKey = r.s_data["KEY"];
					string aesIv = r.s_data["IV"];

					string requete = r.s_data["REQUETE"];
					string token = r.s_data["TOKEN"];

					string statutCible;
					string affectationCible;
					string statut;
					string affectation;
					string action;
					string groClient;
					string groCible;
					string typeData;
					string reference;
					string user;

					istringstream ss(requete);

					getline(ss, action, '*');
					getline(ss, statutCible, '*');
					getline(ss, statut, '*');
					getline(ss, affectationCible, '*');
					getline(ss, affectation, '*');
					getline(ss, groClient, '*');
					getline(ss, groCible, '*');
					getline(ss, typeData, '*');
					getline(ss, reference, '*');
					getline(ss, user, '*');

					string reqFormat;
					string reqFormatBdd;

					
					
					reqFormat = traitement_req_client(token,"4", statutCible, "none", affectationCible, "none", groClient, "none","none",reference, "none");
					reqFormatBdd = traitement_req_bdd(token,"301", statutCible, "none", affectationCible, "none", groClient, "none","none",reference, "none");

					string document = treat_resource(reqFormat, token, 4, reqFormatBdd);			

					if (!document.empty() || document != "")
					{
						p.type = engine_event::PUSH;
						p.s_data["DOCUMENT"] = document;
						p.s_data["TOKEN"] = token;

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

						noeudthor->sendRep(data_encoded);
						
					}
					
					break;
				}
			}	
		}
		default : 
			
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


	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);


	privateKey = CryptoPP::RSA::PrivateKey(params);
	publicKey = CryptoPP::RSA::PublicKey(params);

	engine_event e;
	
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


	
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);


	privateKey = CryptoPP::RSA::PrivateKey(params);
	publicKey = CryptoPP::RSA::PublicKey(params);

	engine_event e;
	
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

	
	AutoSeededRandomPool rng;
	InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng, 1024);



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


	noeudthor->send(outbound_data);

}

void network_interface::send_pull(string& requete, string& encKey, string& token){
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

	e.s_data["REQUETE"]=requete;
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
