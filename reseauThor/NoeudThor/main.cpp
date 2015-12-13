#include <iostream>
#include <boost/asio.hpp>
#include "noeudthor.h"
using namespace std;

int main()
{
	int port = 8082;

	for (; port < 8082+30 ; ++port){
		try
		{
			// Création d'un NoeudThor

			boost::asio::io_service io_service;
			NoeudThor noeudthor(io_service, port);
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
