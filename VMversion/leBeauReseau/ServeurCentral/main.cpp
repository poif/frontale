#include <iostream>
#include <boost/asio.hpp>
#include "serveurcentral.h"
using namespace std;


int main()
{

	int port = 8080;
//	while (port < 8080+30){
		try
		{
			boost::asio::io_service io_service;
			// Création d'un serveur
			ServeurCentral serveur(io_service, port);
			io_service.run();
//			break;
		}
		catch (std::exception& e)
		{
			if (string("bind: Address already in use") == e.what()){
				std::cerr << e.what() << std::endl;
				++port;
			}
			else {
				cout << "Exit with error" << std::endl;
				exit(-1);
			}
		}
//	}

	return 0;
}

