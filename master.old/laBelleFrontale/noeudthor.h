#ifndef NOEUDTHOR_H
#define NOEUDTHOR_H

#include <boost/serialization/list.hpp>
#include "client.h"
#include "network_interface.h"
#include <string>

class network_interface;

class NoeudThor
{
	private:
		int portecoute;
		std::list<Client<NoeudThor>*> toutlemonde;
		tcp::acceptor m_acceptor;
		boost::asio::io_service &io_service;
		Client<NoeudThor>* noeudServeurCentral;
		Client<NoeudThor>* noeudSecureNodeListProvider;
		network_interface* observeur;

		void handle_accept(Client<NoeudThor> *noeud, const boost::system::error_code &error);
		void handle_read(Client<NoeudThor> *noeud, const boost::system::error_code &error);
		void startConnectServeurCentral();
	public:
		NoeudThor(boost::asio::io_service &io_service, int portecoute, network_interface* observeur = NULL);
		void startAccept();
		void traitementDeLaTrame(Trame &t, Client<NoeudThor> *noeudSource);
		void askNeighborList();
		void giveEarPort();
		void askNombreNoeuds();
		void clientLeave(Client<NoeudThor>* leaving);
		void send(string toSend);
		void startConnectSecureNodeListProvider();
};
#endif //NOEUDTHOR_H
