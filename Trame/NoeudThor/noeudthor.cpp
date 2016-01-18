#include "noeudthor.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <stdlib.h>

NoeudThor::NoeudThor(boost::asio::io_service &io_service, int portecoute, std::string ipServeurCentral, int portServeurCentral, std::string ipSecureNodeListProvider, int portSecureNodeListProvider, network_interface* observeur)
	: portecoute(portecoute),
	  ipServeurCentral(ipServeurCentral),
	  portServeurCentral(portServeurCentral),
	  ipSecureNodeListProvider(ipSecureNodeListProvider),
	  portSecureNodeListProvider(portSecureNodeListProvider),
	  m_acceptor(io_service, tcp::endpoint(tcp::v4(), portecoute)),
	  io_service(io_service),
	  observeur(observeur)
{
	cout << "Construction d'un noeudThor" << std::endl;
	srand(time(NULL));
	startConnectServeurCentral();
	startConnectSecureNodeListProvider();
	startAccept();
	giveEarPort();
	askNeighborList();
	sleep(1);
	askNombreNoeuds();
	sleep(1);
	askVoisins();
}


void NoeudThor::startConnectServeurCentral()
{
	noeudServeurCentral = new Client<NoeudThor>(this, io_service);
	tcp::endpoint endpoint(boost::asio::ip::address::from_string(ipServeurCentral), portServeurCentral);
	cout << "Connection au serveur central" << std::endl;
	noeudServeurCentral->getSocket().connect(endpoint);
	noeudServeurCentral->startRead();
}

void NoeudThor::startConnectSecureNodeListProvider()
{
	noeudSecureNodeListProvider = new Client<NoeudThor>(this, io_service);
	tcp::endpoint endpoint(boost::asio::ip::address::from_string(ipSecureNodeListProvider), portSecureNodeListProvider);
	cout << "Connection au Secure Node List Provider" << std::endl;
	noeudSecureNodeListProvider->getSocket().connect(endpoint);
	noeudSecureNodeListProvider->startRead();
}

void NoeudThor::giveEarPort(){
	cout << "Le NoeudThor donne son port d'écoute au serveur." << std::endl;
	Trame t(-1, std::to_string(portecoute));
	cout << "/*********" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
//	boost::asio::streambuf buf;
//	ostream os(&buf);
//	boost::archive::binary_oarchive archiveBinaire(os);
//	archiveBinaire << t;

	noeudSecureNodeListProvider->send(t);
}

void NoeudThor::askNeighborList(){
	cout << "Le NoeudThor demande la liste d'ip:portd'ecoute des autres noeudsThors / frontales" << std::endl;
	Trame t(-2, "");
	cout << "/*********" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
//	boost::asio::streambuf buf;
//	ostream os(&buf);
//	boost::archive::binary_oarchive archiveBinaire(os);
//	archiveBinaire << t;

	noeudSecureNodeListProvider->send(t);
}


void NoeudThor::askNombreNoeuds(){
	cout << "Le NoeudThor demande le nombre de noeuds du réseau" << std::endl;
	Trame t(-3, "");
	cout << "/*********" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;

	noeudSecureNodeListProvider->send(t);
}

void NoeudThor::askVoisins(){
	cout << "Le NoeudThor demande ses voisins" << std::endl;
	Trame t(-4, "");
	cout << "/*********" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	noeudSecureNodeListProvider->send(t);
}

void NoeudThor::clientLeave(Client<NoeudThor> *leaving)
{
	if (leaving == noeudServeurCentral){
		cout << "Le serveur central s'est arreté" << std::endl;
		exit(-1);
	}
	else if (leaving == noeudSecureNodeListProvider){
		cout << "Le Secure Node List Provider s'est arreté" << std::endl;
		exit(-1);
	}
	auto i = std::begin(this->toutlemonde);

	while (i != std::end(this->toutlemonde)) {
		if (*i == leaving)
			i = this->toutlemonde.erase(i);
		else
			++i;
	}
}

void NoeudThor::startAccept()
{
	cout << "Le NoeudThor lance l'acceptation de nouveaux Noeuds" << std::endl;
	Client<NoeudThor>* newNoeud = new Client<NoeudThor>(this, io_service);

	m_acceptor.async_accept(newNoeud->getSocket(),
		boost::bind(&NoeudThor::handle_accept, this, newNoeud,
		boost::asio::placeholders::error));
}

void NoeudThor::traitementDeLaTrame(Trame &t, Client<NoeudThor> *noeudSource)
{
	if (noeudSource == noeudSecureNodeListProvider){
		switch (t.getTTL()) {
			case -2:
			{
				cout << "On a recu la liste d'ip:portd'écoute des autres clients" << std::endl;
				list<pair <string, int> > ipPortEveryBody;

				std::istringstream iStringStream(t.getCommande());
				boost::archive::text_iarchive iTextArchive(iStringStream);
				iTextArchive >> ipPortEveryBody;

				bool found;
				for(pair <string, int> trucl : ipPortEveryBody){
					found=false;
					for (auto it = toutlemonde.begin(); it != toutlemonde.end() && found!=true; it++){
						if ((*it)->getIpStr() == trucl.first && (*it)->getPort() == trucl.second){
							cout << "nous somme déjà connectés à ce client (" << trucl.first << ":" << trucl.second << ")" << std::endl;
							found = true;
						}
					}
					if (found == false){
						cout << "On était pas connecté donc on se connecte" << std::endl;
						auto cli = new Client<NoeudThor>(this, io_service);
						tcp::endpoint endpoint(boost::asio::ip::address::from_string(trucl.first), trucl.second);
						cli->getSocket().connect(endpoint);
						cli->startRead();
						toutlemonde.push_front(cli);
					}
					cout << "IT : " << trucl.first << std::endl;
					cout << "Port : " << trucl.second << std::endl;
				}
				cout << "Enregistrement des différents noeuds effectué" << std::endl;
				break;
			}
			case -3:
			{
				cout << "On a recu le nombre de Noeuds du réseau" << std::endl;
				int nombre;
				sscanf(t.getCommande().c_str(), "%d", &nombre);
				cout << "Il y a " << nombre << "noeuds sur le réseau." << std::endl;
				break;
			}
			case -4:
			{
				cout << "On a recu nos voisins" << std::endl;
				list<pair <string, int> > ipPortVoisins;

				std::istringstream iStringStream(t.getCommande());
				boost::archive::text_iarchive iTextArchive(iStringStream);
				iTextArchive >> ipPortVoisins;

				bool next=false;
				for(pair <string, int> trucl : ipPortVoisins){
					for (auto it = toutlemonde.begin(); it != toutlemonde.end(); it++){
						if ((*it)->getIpStr() == trucl.first && (*it)->getPort() == trucl.second){
							if (next == false){
								this->previous=(*it);
								next = true;
							}
							else {
								this->next=(*it);
							}
						}
					}
				}
				cout << "Enregistrement des voisins effectué" << std::endl;
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else if(noeudSource == noeudServeurCentral){
		switch (t.getTTL()) {
			case 0:
			{
				//observeur->tor_recieve(t.getCommande());
				break;
			}
		}
	}
	else{
		if(t.getTTL() == 0) {
			noeudServeurCentral->send(t);
		}
		if(t.getTTL() == -1) {
			noeudServeurCentral->send(t);
		}
		else if (t.getTTL() > 0)
		{
			t.setTTL(t.getTTL()-1);
			int number = rand()%toutlemonde.size();
			auto i = toutlemonde.begin();
			std::advance(i, number);
			(*i)->send(t);
		}
		else if (t.getTTL() < -1)
		{
			t.setTTL(t.getTTL()+1);
			this->next->send(t);
		}
	}
}

void NoeudThor::handle_accept(Client<NoeudThor> *noeud, const boost::system::error_code &error)
{
	if (!error)
	{
		std::cout << "Nouveau Client se connecte !" << std::endl;
		toutlemonde.push_back(noeud);
		noeud->startRead();
		startAccept(); // (5)
	}
}

void NoeudThor::send(string toSend)
{
	std::cout << "Envoi d'une string depuis la frontale" << std::endl;
	Trame t;
	t.setCommande(toSend);
	t.setTTL(rand()%toutlemonde.size());
	auto i = toutlemonde.begin();
	std::advance(i, rand()%toutlemonde.size());
	(*i)->send(t);
}

void NoeudThor::sendRep(string toSend)
{
	std::cout << "Envoi d'une réponse depuis la frontale" << std::endl;
	Trame t;
	t.setCommande(toSend);
	t.setTTL(-1*(toutlemonde.size()+(rand()%toutlemonde.size())));
	next->send(t);
}
