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

using boost::asio::ip::udp;

enum { max_length = 1024 };

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
    istringstream archive_stream(str_data);
    boost::archive::text_iarchive archive(archive_stream);

    engine_event e;
    archive >> e;
    ostringstream archive_stream;
    switch(e.type){
      case engine_event::LOOK:{
        engine_event r;
      string *finalList = new string[2];
      int nRemote = e.i_data["CHALLENGE"];
      string pubStringRemote = e.s_data["PUB"];
      string affectationReq = e.s_data["AFFECTATION"];
      /*Traitement de la requete */
      finalList = traitement(affectationReq);
      string hashStatList = finalList[0];
      string nomList = finalList[1];


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
          const string &data_encoded = encrypto_rsa(outbound_data, publicRemoteKey);

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
      std::cerr << "Usage: blocking_udp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server(io_service, atoi(argv[1]));
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}


