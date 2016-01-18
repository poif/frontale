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
	
	startAccept();
}

void ServeurCentral::startAccept()
{	
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
		
		toutlemonde.push_back(noeud);
		noeud->startRead();
		startAccept();
		
	}
}

void ServeurCentral::traitementDeLaTrame(Trame &t, Client<ServeurCentral> *noeudSource)
{
	
	t.setTTL(0);
	for(auto cl : this->toutlemonde){
		Client<ServeurCentral> *cli = cl;
		if (cli != noeudSource){
			cli->send(t);
		}
	}
}

void ServeurCentral::clientLeave(Client<ServeurCentral> *leaving)
{
	
	auto i = std::begin(this->toutlemonde);

	while (i != std::end(this->toutlemonde)) {
		if (*i == leaving)
			i = this->toutlemonde.erase(i);
		else
			++i;
	}
}
