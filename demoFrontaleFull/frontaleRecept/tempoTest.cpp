#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <vector>
#include <iostream>
#include <cstdlib>

#include <string>

#include <crypto++/rsa.h>
#include <crypto++/aes.h>
#include <crypto++/osrng.h>
#include <crypto++/base64.h>
#include <crypto++/files.h>
#include <crypto++/modes.h>
#include <crypto++/filters.h>
#include <cryptopp/hex.h>

#include "traitement.h"
#include "engine_event.h"

using boost::asio::ip::udp;
using namespace CryptoPP; 
using namespace std;

enum { max_length = 2048 };

string encrypto_aes(SecByteBlock key, byte* iv, string& plain){

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

string encrypto_rsa(string& plain, RSA::PublicKey pubRemote){

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

string* aesKeyToS(SecByteBlock key, byte* iv){
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

void server(boost::asio::io_service& io_service, unsigned short port)
{
  udp::socket sock(io_service, udp::endpoint(udp::v4(), port));
  for (;;)
  {
    char data[max_length];
    udp::endpoint sender_endpoint;
    size_t length = sock.receive_from(
    boost::asio::buffer(data, max_length), sender_endpoint);

    string str_data(data, length);
    istringstream archive_streamIn(str_data);
    boost::archive::text_iarchive archive(archive_streamIn);

    engine_event e;
    archive >> e;
    ostringstream archive_stream;
    ostringstream archive_streamOut;
    switch(e.type){
      case engine_event::LOOK:{
        engine_event r;
        engine_event p;
      string *finalList = new string[2];
      int nRemote = e.i_data["CHALLENGE"];
      string pubStringRemote = e.s_data["PUB"];
      string affectationReq = e.s_data["AFFECTATION"];
      /*Traitement de la requete */
      finalList = traitement_look(affectationReq);
      string hashStatList = finalList[0];
      cout << hashStatList << endl;
      string nomList = finalList[1];
      cout << nomList << endl;

      if (!hashStatList.empty() || hashStatList != "")
      {
        r.type = engine_event::SHOW;
        r.i_data["CHALL"] = nRemote;
        r.s_data["NOM"] = nomList;
        r.s_data["HSTATUT"] = hashStatList;

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

          RSA::PublicKey publicRemoteKey;
          publicRemoteKey.Load(ss2);
          cout << "Message reponse avant chiffrement avec la cle publique de frontale1 :\n\n" << outbound_data << "\n\n";

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

          //const string &data_encoded = encrypto_rsa(outbound_data, publicRemoteKey);
          cout << "Message pret a etre envoye :\n\n" << data_encoded << "\n";
          sock.send_to(boost::asio::buffer(data_encoded.data(), data_encoded.size()), sender_endpoint);
          //sendTor(outbound_data);
        }
        
        break;
      }
    }


    
  }
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      cerr << "Usage: blocking_udp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server(io_service, atoi(argv[1]));
  }
  catch (exception& e)
  {
    cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}


