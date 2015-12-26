#ifndef CLIENT_H
#define CLIENT_H


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "trame.h"

#include "client.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

template<class TObserveur> class Client
{
	private:
		TObserveur* observeur;
		tcp::socket socketTcp;
		int port;
		boost::array<char, 2048> buffer;
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive& ar, const unsigned int version){
			if(version){};
			//	ar & boost::serialization::base_object<classe_mere>(*this);    // sérialisation de la classe mère
			ar & socketTcp.remote_endpoint().address().to_string() & port;                                    // sérialisation des éléments propres à l'objet courant
		}
		void handle_read(const boost::system::error_code &error);
	public:
		Client(TObserveur* observeur, boost::asio::io_service &io_service);
		void handle_write(Client *noeud, const boost::system::error_code &error);
		tcp::socket& getSocket();
		string getIpStr();
		void startRead();
		int getPort() const;
		void setPort(int value);
		void send(Trame& t);
};

template<class TObserveur> Client<TObserveur>::Client(TObserveur *observeur, boost::asio::io_service &io_service): observeur(observeur), socketTcp(io_service), port(0)
{
}


template<class TObserveur>
tcp::socket& Client<TObserveur>::getSocket()
{
	return socketTcp;
}

template<class TObserveur>
string Client<TObserveur>::getIpStr()
{
	return socketTcp.remote_endpoint().address().to_string();
}

template<class TObserveur>
void Client<TObserveur>::startRead()
{
	socketTcp.async_read_some(boost::asio::buffer(buffer),
				boost::bind(&Client::handle_read, this,
				boost::asio::placeholders::error)
				);
}

template<class TObserveur>
int Client<TObserveur>::getPort() const
{
	return port;
}

template<class TObserveur>
void Client<TObserveur>::setPort(int value)
{
	port = value;
}

template<class TObserveur>
void Client<TObserveur>::send(Trame &t)
{
	boost::asio::streambuf buf;
	ostream os(&buf);
	boost::archive::binary_oarchive archiveBinaire(os);
	archiveBinaire << t;
	socketTcp.send(boost::asio::buffer(buf.data()));
}

template<class TObserveur>
void Client<TObserveur>::handle_read(const boost::system::error_code& error) {
	if (!error)
	{
		try {
			Trame t;
			string reception(buffer.data(), buffer.size());
			startRead();
			istringstream is(reception);
			boost::archive::binary_iarchive archiveBinaire(is);
			archiveBinaire >> t;
			observeur->traitementDeLaTrame(t, this);

		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	else
	{
		socketTcp.close();
		observeur->clientLeave(this);
	}
}


#endif // CLIENT_H