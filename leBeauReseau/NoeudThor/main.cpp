#include <iostream>
#include <boost/asio.hpp>
#include "noeudthor.h"
using namespace std;

int main(int argc, char **argv)
{
	if (argc != 5){
		cout << "Usage : "<< argv[0] << " ip_serveurCentral port_serveurCentral ip_secureNodeListProvider port_secureNodeListProvider"<< std::endl;
		cout << "Lancement avec les parametres par defaut : " << argv[0] << " 127.0.0.1 8080 127.0.0.1 8081" << std::endl;
	}
	int port = 8082;

	int portServeurCentral = atoi(argv[2]);
	int portSecureNodeListProvider = atoi(argv[4]);

	for (; port < 8082+30 ; ++port){
		try
		{
			// Création d'un NoeudThor
			NoeudThor *noeudthor;
			boost::asio::io_service io_service;
			if (argc == 5){
				noeudthor = new NoeudThor(io_service, port, argv[1], portServeurCentral, argv[3], portSecureNodeListProvider);
			}
			else {
				noeudthor = new NoeudThor(io_service, port);
			}
			io_service.run();
			break;
		}
		catch (std::exception& e)
		{
			if (string("bind: Address already in use") == e.what()){
				std::cerr << e.what() << std::endl;
			}
			else {
				cout << "Exit with error" << std::endl;
				std::cerr << e.what() << std::endl;
				exit(-1);
			}
		}
	}

	return 0;
}
