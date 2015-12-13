#include "trame.h"

Trame::Trame(int16_t TTL, string commande) : TTL(TTL), commande(commande)
{

}

Trame::Trame()
{

}

int16_t Trame::getTTL() const
{
	return TTL;
}

void Trame::setTTL(const int16_t &value)
{
	TTL = value;
}

string Trame::getCommande() const
{
	return commande;
}

void Trame::setCommande(const string &value)
{
	commande = value;
}