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
		/**
		 * accepter des connections et l'ajouter à sa liste de file descriptor de personnes connectée
		 * demander l'adresse ip et le port à la machine correspondante TTL = -2
		 * récupérer l'ip et le port dans sa liste / ldap
		 * renvoie de la liste de noeuds manière sérialisée
		 * ajout à la liste
		 */

		/** lecture sur la socket : asynchrone appelle fonction de callback **/

		/** foncion de callback qui :
		 *		deserialise la trame
		 *		si ttl == -1 renvoie la liste ip:port
		 *		sinon broadcast la trame
		 */
		void traitementDeLaTrame(Trame& t, Client<ServeurCentral>*noeudSource);
		void sendTrameToRecipient(Trame& t);

		void sendNbNoeuds(Client<ServeurCentral> *to);
		void clientLeave(Client<ServeurCentral> *leaving);
		void sendNbNoeudsToAll();
};


#endif // SERVEURCENTRAL_H
