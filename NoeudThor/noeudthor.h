#ifndef NOEUDTHOR_H
#define NOEUDTHOR_H

#include <boost/serialization/list.hpp>
#include "client.h"
#include <string>

class network_interface;

class NoeudThor
{
	private:
		int portecoute;
		std::string ipServeurCentral;
		int portServeurCentral;
		std::string ipSecureNodeListProvider;
		int portSecureNodeListProvider;
		std::list<Client<NoeudThor>*> toutlemonde;
		tcp::acceptor m_acceptor;
		boost::asio::io_service &io_service;
		Client<NoeudThor>* noeudServeurCentral;
		Client<NoeudThor>* noeudSecureNodeListProvider;

		Client<NoeudThor>* previous;
		Client<NoeudThor>* next;

		network_interface* observeur;

		void handle_accept(Client<NoeudThor> *noeud, const boost::system::error_code &error);
		void handle_read(Client<NoeudThor> *noeud, const boost::system::error_code &error);
		void startConnectServeurCentral();
	public:
		NoeudThor(boost::asio::io_service &io_service, int portecoute, std::string ipServeurCentral="127.0.0.1", int portServeurCentral=8080, std::string ipSecureNodeListProvider="127.0.0.1", int portSecureNodeListProvider=8081, network_interface* observeur = NULL);
		void startAccept();
		void traitementDeLaTrame(Trame &t, Client<NoeudThor> *noeudSource);
		void askNeighborList();
		void giveEarPort();
        void giveEarPortToAll();
		void askNombreNoeuds();
		void askVoisins();
		void clientLeave(Client<NoeudThor>* leaving);
		void send(string toSend);
		void sendRep(string toSend);
		void startConnectSecureNodeListProvider();
};
#endif //NOEUDTHOR_H
