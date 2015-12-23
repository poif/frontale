#include "fusionneur.h"
#include <iostream>
#include <sstream>
#include <openssl/sha.h>

using namespace std;
Fusionneur* Fusionneur::_instance = nullptr;

Fusionneur::Fusionneur()
{
	t0=time(NULL);
	srand(t0);
}

Fusionneur* Fusionneur::getInstance()
{
	if (Fusionneur::_instance == nullptr){
		_instance=new Fusionneur;
	}
	return _instance;
}

std::string Fusionneur::GenToken()
{
	ostringstream tokenContainer;
	tokenContainer << time(NULL)-t0<< rand()%100;

	SHA_CTX ctx;
	SHA1_Init(&ctx);
	char hashChar[SHA_DIGEST_LENGTH];
	SHA1_Update(&ctx,tokenContainer.str().c_str(),tokenContainer.str().length());
	SHA1_Final((unsigned char*)hashChar,&ctx);
	string hash = string(hashChar, SHA_DIGEST_LENGTH);
	return hash;
}

//a appeler avant de send au réseau: créé l'espace de stockage des résultats avant défloutage
//créé aussi le timer qu'on lancera avec startTimer('token') juste après l'envoi des données sur le réseau
void Fusionneur::createDataStorage(string token)
{
	if (tokenToTimer[token] == NULL){
		tokenToTimer[token]=new TimerToken(token);
		tokenToTimer[token]->setInterval(2000);
		tokenToTimer[token]->setSingleShot(true);
		connect(tokenToTimer[token], SIGNAL(timeout()), this, SLOT(dataRecieved(tokenToTimer[token].token)));
	}
	else {
		cerr << "Erreur la map n'est pas censé avoir un TimerToken à cet emplacement avant d'avoir recu ce token" << endl;
	}
	if (tokenMsgList[token] == NULL){
		tokenMsgList[token] = new list<string>();
	}
	else {
		// Ne devrait jamais s'afficher
		cerr << "Erreur la map n'est pas censé avoir une liste à cet emplacement avant d'avoir recu ce token" << endl;
	}
}

void Fusionneur::startTimer(string token)
{
	if (tokenToTimer[token] != NULL){
		tokenToTimer[token]->start();
	}
}

void Fusionneur::doNothingForTestingPurpose(string token)
{
	cout << "Le slot à été appelé correctement avec le token" << token << endl;
}
