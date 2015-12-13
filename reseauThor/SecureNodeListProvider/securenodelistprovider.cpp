#include "securenodelistprovider.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

SecureNodeListProvider::SecureNodeListProvider(boost::asio::io_service &io_service, int portecoute)
	: portecoute(portecoute),
	  m_acceptor(io_service, tcp::endpoint(tcp::v4(), portecoute)),
	  io_service(io_service)
{
	cout << "Construction du serveur central" << std::endl;
	startAccept();
}

void SecureNodeListProvider::startAccept()
{
	cout << "Serveur accepte des clients" << std::endl;
	Client<SecureNodeListProvider>* newNoeud = new Client<SecureNodeListProvider>(this, io_service);
	m_acceptor.async_accept(newNoeud->getSocket(),
		boost::bind(&SecureNodeListProvider::handle_accept,
					this,
					newNoeud,
					boost::asio::placeholders::error));
}

void SecureNodeListProvider::handle_accept(Client<SecureNodeListProvider> *noeud, const boost::system::error_code &error)
{
	if (!error)
	{
		std::cout << "Un client est accepté !" << std::endl;
		toutlemonde.push_back(noeud);
		noeud->startRead();
		startAccept();
	}
}

void SecureNodeListProvider::sendIpPortAllNodes(Client<SecureNodeListProvider> *to){
	cout << "Envoi de la liste d'ip:portd'ecoute" << std::endl;
	std::ostringstream oStringStream;
	boost::archive::text_oarchive oTextArchive(oStringStream);

	list<pair <string, int> > ipPortEveryBody;
	for(auto cl : this->toutlemonde){
		Client<SecureNodeListProvider> *cli = cl;
		if ((cli->getIpStr() != to->getIpStr() || cli->getPort() != to->getPort()) && cli->getPort()!=0){
			ipPortEveryBody.insert(ipPortEveryBody.end(), pair<string, int>(cli->getIpStr(), cli->getPort()));

		}
	}
	oTextArchive << ipPortEveryBody;    // sérialisation de t

	Trame t(-2, oStringStream.str());
	cout << "/*********Trame réponse" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	boost::asio::streambuf buf;
	ostream os(&buf);
	boost::archive::binary_oarchive archiveBinaire(os);
	archiveBinaire << t;

	to->send(t);
}

void SecureNodeListProvider::traitementDeLaTrame(Trame &t, Client<SecureNodeListProvider> *noeudSource)
{
	cout << "/********* Trame Recue" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	const int TTL_RENSEIGNE_NO_PORT=-1;
	const int TTL_ASK_LIST_IP_PORT=-2;
	const int TTL_ASK_NB_NOEUDS=-3;
	/*Si le noeud est complété*/
	if (noeudSource->getPort() == 0){
		if (t.getTTL() != TTL_RENSEIGNE_NO_PORT){
			cout << "Erreur un client essaye de communiquer sans avoir de port d'écoute ouvert/renseigné" << std::endl;
			return;
		}
		else {
			noeudSource->setPort(stoi(t.getCommande(), NULL, 10));
			cout << "Le noeud à renseigné son port" << std::endl;
		}
	}
	else {
		switch (t.getTTL()) {
			case TTL_ASK_LIST_IP_PORT:
				cout << "Le noeud demande la liste d'ip:portd'écoute" << std::endl;
				sendIpPortAllNodes(noeudSource);
				break;
			case TTL_ASK_NB_NOEUDS:
				cout << "Le noeud demande le nombre de noeuds du réseau" << std::endl;
				sendNbNoeuds(noeudSource);
				break;
			default:
				cout << "This case should never append" << std::endl;
				break;
		}
	}
}

void SecureNodeListProvider::sendNbNoeuds(Client<SecureNodeListProvider> *to){
	int nbr = 0;
	for(auto cl : this->toutlemonde){
		Client<SecureNodeListProvider> *cli = cl;
		if (cli->getPort() != 0)
			++nbr;
	}

	Trame t(-3, std::to_string(nbr));
	boost::asio::streambuf buf;
	ostream os(&buf);
	boost::archive::binary_oarchive archiveBinaire(os);
	archiveBinaire << t;
	to->send(t);
}

void SecureNodeListProvider::sendTrameToRecipient(Trame &t)
{
	t.setTTL(0);
	for (auto n : toutlemonde){
		Client<SecureNodeListProvider>* p = n;
		p->send(t);
	}
}

void SecureNodeListProvider::clientLeave(Client<SecureNodeListProvider> *leaving)
{
	cout << "Client quitte le serveur" << std::endl;
	auto i = std::begin(this->toutlemonde);

	while (i != std::end(this->toutlemonde)) {
		if (*i == leaving)
			i = this->toutlemonde.erase(i);
		else
			++i;
	}
}
