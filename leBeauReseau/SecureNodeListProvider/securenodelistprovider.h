#ifndef SERVEURCENTRAL_H
#define SERVEURCENTRAL_H

#include <string>
#include <boost/asio.hpp>
#include <boost/serialization/list.hpp>
#include "client.h"
#include "trame.h"

class SecureNodeListProvider
{
	private:
		int portecoute;
		std::list<Client<SecureNodeListProvider>*> toutlemonde;
		tcp::acceptor m_acceptor;
		boost::asio::io_service &io_service;
		void envoyerIpPortTousConnecte(Client<SecureNodeListProvider> *n);
		void sendIpPortAllNodes(Client<SecureNodeListProvider> *to);
		void startAccept();
		void handle_accept(Client<SecureNodeListProvider>* noeud, const boost::system::error_code& error);
		void sendVoisins(Client<SecureNodeListProvider> *to);
		void sendNbNoeuds(Client<SecureNodeListProvider> *to);
		void sendNbNoeudsToAll();
        void sendTrameToRecipient(Trame &t);
	public:
		SecureNodeListProvider(boost::asio::io_service &io_service, int portecoute);
		void traitementDeLaTrame(Trame& t, Client<SecureNodeListProvider>*noeudSource);
		void clientLeave(Client<SecureNodeListProvider> *leaving);
};


#endif // SECURENODELISTPROVIDER_H
