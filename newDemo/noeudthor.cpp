#include "noeudthor.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <stdlib.h>

NoeudThor::NoeudThor(boost::asio::io_service &io_service, int portecoute, network_interface * observeur)
	: portecoute(portecoute),
	  m_acceptor(io_service, tcp::endpoint(tcp::v4(), portecoute)),
	  io_service(io_service),
	  observeur(observeur)
{
	cout << "Construction d'un noeudThor" << std::endl;
	startConnect();
	startAccept();
	giveEarPort();
	askNeighborList();
	askNombreNoeuds();
}
/*
NoeudThor::NoeudThor()
{

}*/


void NoeudThor::startConnect()
{
	noeudServeurCentral = new Client<NoeudThor>(this, io_service);
	tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 8080);
	cout << "Connection au serveur central" << std::endl;
	noeudServeurCentral->getSocket().connect(endpoint);
	noeudServeurCentral->startRead();
}

void NoeudThor::giveEarPort(){
	cout << "Le NoeudThor donne son port d'écoute au serveur." << std::endl;
	Trame t(-1, std::to_string(portecoute));
	cout << "/*********" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	boost::asio::streambuf buf;
	ostream os(&buf);
	boost::archive::binary_oarchive archiveBinaire(os);
	archiveBinaire << t;

	noeudServeurCentral->send(t);
}

void NoeudThor::askNeighborList(){
	cout << "Le NoeudThor demande la liste d'ip:portd'ecoute des autres noeudsThors / frontales" << std::endl;
	Trame t(-2, "");
	cout << "/*********" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	boost::asio::streambuf buf;
	ostream os(&buf);
	boost::archive::binary_oarchive archiveBinaire(os);
	archiveBinaire << t;

	noeudServeurCentral->send(t);
}


void NoeudThor::askNombreNoeuds(){
	cout << "Le NoeudThor demande le nombre de noeuds du réseau" << std::endl;
	Trame t(-3, "");
	cout << "/*********" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl;
	boost::asio::streambuf buf;
	ostream os(&buf);
	boost::archive::binary_oarchive archiveBinaire(os);
	archiveBinaire << t;

	noeudServeurCentral->send(t);
}

void NoeudThor::clientLeave(Client<NoeudThor> *leaving)
{
	if (leaving == noeudServeurCentral){
		cout << "Le serveur central s'est arreté" << std::endl;
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
	if (noeudSource == this->noeudServeurCentral){
		switch (t.getTTL()) {
			case -1:
			{
				break;
			}
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
			case 0:
			{
				observeur->tor_receive(t.getCommande());
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else{
		switch (t.getTTL()) {
			case 0:
			{
				noeudServeurCentral->send(t);
				break;
			}
			default:
			{
				t.setTTL(t.getTTL()-1);
				srand(time(NULL));
				int number = rand()%toutlemonde.size();
				auto i = toutlemonde.begin();
				std::advance(i, number);
				(*i)->send(t);
				break;
			}
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

void NoeudThor::send(std::string toSend)
{
    std::cout << "Envoi d'une string depuis la frontale" << std::endl;
    Trame t;
    t.setCommande(toSend);
    t.setTTL(rand()%toutlemonde.size());
    auto i = toutlemonde.begin();
    std::advance(i, rand()%toutlemonde.size());
    (*i)->send(t);
}
