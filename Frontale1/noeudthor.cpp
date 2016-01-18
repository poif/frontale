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
	
	srand(time(NULL));
	startConnectServeurCentral();
	startConnectSecureNodeListProvider();
	startAccept();
    giveEarPort();
    askNeighborList();

}


void NoeudThor::startConnectServeurCentral()
{
	noeudServeurCentral = new Client<NoeudThor>(this, io_service);
	tcp::endpoint endpoint(boost::asio::ip::address::from_string(ipServeurCentral), portServeurCentral);
	noeudServeurCentral->getSocket().connect(endpoint);
	noeudServeurCentral->startRead();
}

void NoeudThor::startConnectSecureNodeListProvider()
{
	noeudSecureNodeListProvider = new Client<NoeudThor>(this, io_service);
	tcp::endpoint endpoint(boost::asio::ip::address::from_string(ipSecureNodeListProvider), portSecureNodeListProvider);
	noeudSecureNodeListProvider->getSocket().connect(endpoint);
	noeudSecureNodeListProvider->startRead();
}

void NoeudThor::giveEarPort(){
	Trame t(-1, std::to_string(portecoute));


    noeudSecureNodeListProvider->send(t);
}



void NoeudThor::askNeighborList(){
	Trame t(-2, "");


	noeudSecureNodeListProvider->send(t);
}


void NoeudThor::askNombreNoeuds(){
	Trame t(-3, "");


	noeudSecureNodeListProvider->send(t);
}

void NoeudThor::askVoisins(){
	Trame t(-4, "");

	noeudSecureNodeListProvider->send(t);
}

void NoeudThor::clientLeave(Client<NoeudThor> *leaving)
{
	if (leaving == noeudServeurCentral){
		exit(-1);
	}
	else if (leaving == noeudSecureNodeListProvider){
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
				list<pair <string, int> > ipPortEveryBody;

				std::istringstream iStringStream(t.getCommande());
				boost::archive::text_iarchive iTextArchive(iStringStream);
				iTextArchive >> ipPortEveryBody;

                bool found;
                for(pair <string, int> trucl : ipPortEveryBody){
					found=false;
					for (auto it = toutlemonde.begin(); it != toutlemonde.end() && found!=true; it++){
						if ((*it)->getIpStr() == trucl.first && (*it)->getPort() == trucl.second){
							found = true;
						}
					}
					if (found == false){
						auto cli = new Client<NoeudThor>(this, io_service);
						tcp::endpoint endpoint(boost::asio::ip::address::from_string(trucl.first), trucl.second);
						cli->getSocket().connect(endpoint);
                        cli->startRead();
                        Trame t;
                        t.setTTL(-1);
                        t.setCommande(std::to_string(this->portecoute));
                        cli->send(t);
                        cli->setPort(trucl.second);
						toutlemonde.push_front(cli);
					}

                }
          
                askVoisins();
				break;
            }
			case -3:
			{
				int nombre;
				sscanf(t.getCommande().c_str(), "%d", &nombre);
				break;
			}
			case -4:
			{
				list<pair <string, int> > ipPortVoisins;

				std::istringstream iStringStream(t.getCommande());
				boost::archive::text_iarchive iTextArchive(iStringStream);
				iTextArchive >> ipPortVoisins;

                bool isNext=false;
                const int SET_PREVIOUS=0;
                const int SET_NEXT=1;
                const int ALL_SET=2;
                char whatToDo=SET_PREVIOUS;
				for(pair <string, int> trucl : ipPortVoisins){
					for (auto it = toutlemonde.begin(); it != toutlemonde.end(); it++){
						if ((*it)->getIpStr() == trucl.first && (*it)->getPort() == trucl.second){
                            if (whatToDo == SET_PREVIOUS){
                                this->previous=(*it);
                                whatToDo = SET_NEXT;
							}
							else {
                                this->next=(*it);
                                whatToDo = ALL_SET;
                            }
						}
					}
				}
                if (whatToDo != ALL_SET){
                    this->previous=NULL;
                    this->next=NULL;
                }
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
				observeur->tor_receive(t.getCommande());
				break;
			}
		}
	}
	else{
		if(t.getTTL() == 0) {
			noeudServeurCentral->send(t);
        }
        else if (t.getTTL() == -1)
        {
            noeudSource->setPort(stoi(t.getCommande(), NULL, 10));
        }
        else if(t.getTTL() == -2) {
        			observeur->tor_receive(t.getCommande());
        }
		else if (t.getTTL() > 0)
		{
			t.setTTL(t.getTTL()-1);
			int number = rand()%toutlemonde.size();
			auto i = toutlemonde.begin();
			std::advance(i, number);
			(*i)->send(t);
		}
        else if (t.getTTL() < -2)
		{
			t.setTTL(t.getTTL()+1);
            if (this->next != NULL)
                this->next->send(t);
		}
	}
}

void NoeudThor::handle_accept(Client<NoeudThor> *noeud, const boost::system::error_code &error)
{
	if (!error)
	{
        toutlemonde.push_back(noeud);
		noeud->startRead();
        startAccept();
	}
}

void NoeudThor::send(string toSend)
{
	Trame t;
	t.setCommande(toSend);
	t.setTTL(rand()%toutlemonde.size());
	auto i = toutlemonde.begin();
	std::advance(i, rand()%toutlemonde.size());
	(*i)->send(t);
}

void NoeudThor::sendRep(string toSend)
{
	Trame t;
	t.setCommande(toSend);
    t.setTTL(-1*(toutlemonde.size()+(rand()%toutlemonde.size())+2));
	if (next != NULL)
		next->send(t);
}
