#include "serveurcentral.h"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

ServeurCentral::ServeurCentral(boost::asio::io_service &io_service, int portecoute)
	: portecoute(portecoute),
	  m_acceptor(io_service, tcp::endpoint(tcp::v4(), portecoute)),
	  io_service(io_service)
{
	cout << "Construction du serveur central" << std::endl;
	startAccept();
}

void ServeurCentral::startAccept()
{
	cout << "Serveur accepte des clients" << std::endl;
	Client<ServeurCentral>* newNoeud = new Client<ServeurCentral>(this, io_service);
	m_acceptor.async_accept(newNoeud->getSocket(),
		boost::bind(&ServeurCentral::handle_accept,
					this,
					newNoeud,
					boost::asio::placeholders::error));
}

void ServeurCentral::handle_accept(Client<ServeurCentral> *noeud, const boost::system::error_code &error)
{
	if (!error)
	{
		std::cout << "Un client est accepté !" << std::endl;
		toutlemonde.push_back(noeud);
		noeud->startRead();
		startAccept();
	}
}

void ServeurCentral::traitementDeLaTrame(Trame &t, Client<ServeurCentral> *noeudSource)
{
	cout << "/********* Trame Recue" << std::endl <<
			"TTL : " << t.getTTL() << std::endl <<
			"Commande : " << t.getCommande() << std::endl <<
			"*********/" << std::endl << std::endl;
	t.setTTL(0);
	for(auto cl : this->toutlemonde){
		Client<ServeurCentral> *cli = cl;
		cli->send(t);
	}
}

void ServeurCentral::clientLeave(Client<ServeurCentral> *leaving)
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
