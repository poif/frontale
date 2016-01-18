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

	to->send(t);
}

void SecureNodeListProvider::traitementDeLaTrame(Trame &t, Client<SecureNodeListProvider> *noeudSource)
{
	cout << "/********* Trame Recue" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
            "Commande:" << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	const int TTL_RENSEIGNE_NO_PORT=-1;
	const int TTL_ASK_LIST_IP_PORT=-2;
	const int TTL_ASK_NB_NOEUDS=-3;
	const int TTL_ASK_VOISINS=-4;
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
			case TTL_ASK_VOISINS:
				cout << "Le noeud demande ses voisins" << std::endl;
				sendVoisins(noeudSource);
				break;
			default:
				cout << "This case should never append" << std::endl;
				break;
		}
	}
}

void SecureNodeListProvider::sendVoisins(Client<SecureNodeListProvider> *to){
	cout << "Envoi des voisins d'un noeud" << std::endl;
	std::ostringstream oStringStream;
	std::ostringstream oStringStreamPrevious;
	std::ostringstream oStringStreamNext;
	boost::archive::text_oarchive oTextArchive(oStringStream);
	boost::archive::text_oarchive oTextArchivePrevious(oStringStreamPrevious);
	boost::archive::text_oarchive oTextArchiveNext(oStringStreamNext);

	list<pair <string, int> > ipPortVoisins;
	Client<SecureNodeListProvider> *previous=NULL;
	list<pair <string, int> > ipPortOfPreviousNode;
	Client<SecureNodeListProvider> *next=NULL;
	list<pair <string, int> > ipPortOfNextNode;

	if (this->toutlemonde.size() == 1){
		ipPortVoisins.push_front(pair<string, int>("0.0.0.0", 0));
		ipPortVoisins.push_back(pair<string, int>("0.0.0.0", 0));
	}
	else{
		// Completion le la liste de voisins et enregistrement des précédents et suivants
		auto it=this->toutlemonde.begin();
		auto tmp=it;
		for(; it != this->toutlemonde.end(); ++it){
			Client<SecureNodeListProvider> *cli = *it;
			if (cli == to){
				tmp=it;
				if (++tmp == this->toutlemonde.end()){
					tmp=this->toutlemonde.begin();
				}
				next=*tmp;
				cout << "Next trouvé " << next->getIpStr() << ":" << next->getPort();
				ipPortVoisins.push_back(pair<string, int>(next->getIpStr(), next->getPort()));
			}
		}
		auto it2=this->toutlemonde.rbegin();
		auto tmp2=it2;
		for(; it2 != this->toutlemonde.rend(); ++it2){
			Client<SecureNodeListProvider> *cli = *it2;
			if (cli == to){
				tmp2=it2;
				if (++tmp2 == this->toutlemonde.rend()){
					tmp2=this->toutlemonde.rbegin();
				}
				previous=*tmp2;
				cout << "Previous trouvé " << previous->getIpStr() << ":" << previous->getPort();
				ipPortVoisins.push_front(pair<string, int>(previous->getIpStr(), previous->getPort()));
			}
		}

		//completion des listes des deux voisins

		auto it3=this->toutlemonde.begin();
		auto tmp3=it3;
		for(; it3 != this->toutlemonde.end(); ++it3){
			Client<SecureNodeListProvider> *cli = *it3;
			if (cli == next){
				tmp3=it3;
				if (++tmp3 == this->toutlemonde.end()){
					tmp3=this->toutlemonde.begin();
				}
				ipPortOfNextNode.push_back(pair<string, int>((*tmp3)->getIpStr(), (*tmp3)->getPort()));
			}
			if (cli == previous){
				tmp3=it3;
				if (++tmp3 == this->toutlemonde.end()){
					tmp3=this->toutlemonde.begin();
				}
				ipPortOfPreviousNode.push_back(pair<string, int>((*tmp3)->getIpStr(), (*tmp3)->getPort()));
			}
		}
		auto it4=this->toutlemonde.rbegin();
		auto tmp4=it4;
		for(; it4 != this->toutlemonde.rend(); ++it4){
			Client<SecureNodeListProvider> *cli = *it4;
			if (cli == next){
				tmp4=it4;
				if (++tmp4 == this->toutlemonde.rend()){
					tmp4=this->toutlemonde.rbegin();
				}
				ipPortOfNextNode.push_front(pair<string, int>((*tmp4)->getIpStr(), (*tmp4)->getPort()));
			}
			if (cli == previous){
				tmp4=it4;
				if (++tmp4 == this->toutlemonde.rend()){
					tmp4=this->toutlemonde.rbegin();
				}
				ipPortOfPreviousNode.push_front(pair<string, int>((*tmp4)->getIpStr(), (*tmp4)->getPort()));
			}
		}
	}

	oTextArchive << ipPortVoisins;
	Trame t(-4, oStringStream.str());
	cout << "/*********Trame réponse" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	to->send(t);

	if (this->toutlemonde.size() != 1){
		oTextArchivePrevious << ipPortOfPreviousNode;
		Trame t2(-4, oStringStreamPrevious.str());
		cout << "/*********Trame Précédent" << std::endl <<
				"TTL : " << t2.getTTL() << std::endl <<
				"Commande : " << t2.getCommande() << std::endl <<
				"*********/" << std::endl;
        previous->send(t2);

		oTextArchiveNext << ipPortOfNextNode;
		Trame t3(-4, oStringStreamNext.str());
		cout << "/*********Trame Suivant" << std::endl <<
				"TTL : " << t3.getTTL() << std::endl <<
				"Commande : " << t3.getCommande() << std::endl <<
				"*********/" << std::endl;
        next->send(t3);
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
	cout << "/*********Trame réponse" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
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

	std::ostringstream oStringStream;
	std::ostringstream oStringStreamPrevious;
	std::ostringstream oStringStreamNext;
	boost::archive::text_oarchive oTextArchive(oStringStream);
	boost::archive::text_oarchive oTextArchivePrevious(oStringStreamPrevious);
	boost::archive::text_oarchive oTextArchiveNext(oStringStreamNext);

	list<pair <string, int> > ipPortVoisins;
	Client<SecureNodeListProvider> *previous=NULL;
	list<pair <string, int> > ipPortOfPreviousNode;
	Client<SecureNodeListProvider> *next=NULL;
	list<pair <string, int> > ipPortOfNextNode;

	if (this->toutlemonde.size() == 2){
		auto i = std::begin(this->toutlemonde);

		while (i != std::end(this->toutlemonde)) {
			if (*i == leaving)
				i = this->toutlemonde.erase(i);
			else
				++i;
		}
		i = std::begin(this->toutlemonde);

		ipPortVoisins.push_front(pair<string, int>("0.0.0.0", 0));
		ipPortVoisins.push_back(pair<string, int>("0.0.0.0", 0));

		oTextArchive << ipPortVoisins;
		Trame t(-4, oStringStream.str());
		cout << "/*********Trame réponse" << std::endl <<
				"TTL : " << t.getTTL() << std::endl <<
				"Commande : " << t.getCommande() << std::endl <<
				"*********/" << std::endl;
		(*i)->send(t);
	}
	else{
		// On trouve le précédent et suivant
		auto it=this->toutlemonde.begin();
		auto tmp=it;
		for(; it != this->toutlemonde.end(); ++it){
			Client<SecureNodeListProvider> *cli = *it;
			if (cli == leaving){
				tmp=it;
				if (++tmp == this->toutlemonde.end()){
					tmp=this->toutlemonde.begin();
				}
				next=*tmp;
				cout << "Next trouvé " << next->getIpStr() << ":" << next->getPort();
			}
		}
		auto it2=this->toutlemonde.rbegin();
		auto tmp2=it2;
		for(; it2 != this->toutlemonde.rend(); ++it2){
			Client<SecureNodeListProvider> *cli = *it2;
			if (cli == leaving){
				tmp2=it2;
				if (++tmp2 == this->toutlemonde.rend()){
					tmp2=this->toutlemonde.rbegin();
				}
				previous=*tmp2;
				cout << "Previous trouvé " << previous->getIpStr() << ":" << previous->getPort();
			}
		}

		// on delete notre client de la liste :
		auto i = std::begin(this->toutlemonde);

		while (i != std::end(this->toutlemonde)) {
			if (*i == leaving)
				i = this->toutlemonde.erase(i);
			else
				++i;
		}

		//completion des listes des deux voisins
		auto it3=this->toutlemonde.begin();
		auto tmp3=it3;
		for(; it3 != this->toutlemonde.end(); ++it3){
			Client<SecureNodeListProvider> *cli = *it3;
			if (cli == next){
				tmp3=it3;
				if (++tmp3 == this->toutlemonde.end()){
					tmp3=this->toutlemonde.begin();
				}
				ipPortOfNextNode.push_back(pair<string, int>((*tmp3)->getIpStr(), (*tmp3)->getPort()));
			}
			if (cli == previous){
				tmp3=it3;
				if (++tmp3 == this->toutlemonde.end()){
					tmp3=this->toutlemonde.begin();
				}
				ipPortOfPreviousNode.push_back(pair<string, int>((*tmp3)->getIpStr(), (*tmp3)->getPort()));
			}
		}
		auto it4=this->toutlemonde.rbegin();
		auto tmp4=it4;
		for(; it4 != this->toutlemonde.rend(); ++it4){
			Client<SecureNodeListProvider> *cli = *it4;
			if (cli == next){
				tmp4=it4;
				if (++tmp4 == this->toutlemonde.rend()){
					tmp4=this->toutlemonde.rbegin();
				}
				ipPortOfNextNode.push_front(pair<string, int>((*tmp4)->getIpStr(), (*tmp4)->getPort()));
			}
			if (cli == previous){
				tmp4=it4;
				if (++tmp4 == this->toutlemonde.rend()){
					tmp4=this->toutlemonde.rbegin();
				}
				ipPortOfPreviousNode.push_front(pair<string, int>((*tmp4)->getIpStr(), (*tmp4)->getPort()));
			}
		}

		// envoi des information aux noeuds toujours présents :
		oTextArchivePrevious << ipPortOfPreviousNode;
		Trame t2(-4, oStringStreamPrevious.str());
		cout << "/*********Trame Précédent" << std::endl <<
				"TTL : " << t2.getTTL() << std::endl <<
				"Commande : " << t2.getCommande() << std::endl <<
				"*********/" << std::endl;

		previous->send(t2);

		oTextArchiveNext << ipPortOfNextNode;
		Trame t3(-4, oStringStreamNext.str());
		cout << "/*********Trame Suivant" << std::endl <<
				"TTL : " << t3.getTTL() << std::endl <<
				"Commande : " << t3.getCommande() << std::endl <<
				"*********/" << std::endl;
		next->send(t3);
	}


}
