#ifndef SERVEURCENTRAL_H
#define SERVEURCENTRAL_H

#include <string>
#include <boost/asio.hpp>
#include <boost/serialization/list.hpp>
#include "client.h"
#include "trame.h"

class ServeurCentral
{
	private:
		int portecoute;
		std::list<Client<ServeurCentral>*> toutlemonde;
		tcp::acceptor m_acceptor;
		boost::asio::io_service &io_service;
		void envoyerIpPortTousConnecte(Client<ServeurCentral> *n);
		void sendIpPortAllNodes(Client<ServeurCentral> *to);
	public:
		ServeurCentral(boost::asio::io_service &io_service, int portecoute);
		void startAccept();
		void handle_accept(Client<ServeurCentral>* noeud, const boost::system::error_code& error);
		void traitementDeLaTrame(Trame& t, Client<ServeurCentral>*noeudSource);
		void clientLeave(Client<ServeurCentral> *leaving);
};


#endif // SERVEURCENTRAL_H
