#ifndef TRAME_H
#define TRAME_H

#include <stdint.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp> //maybe useless
#include <boost/serialization/base_object.hpp> //maybe useless

using namespace std;

class Trame
{
	private:
		int16_t TTL;
		string commande;
		friend class boost::serialization::access;
		template<class Archive> void serialize(Archive& ar, const unsigned int version){
			if(version){};
			//	ar & boost::serialization::base_object<classe_mere>(*this);    // sérialisation de la classe mère
			ar & TTL & commande;                                    // sérialisation des éléments propres à l'objet courant
		}
	public:
		Trame(int16_t TTL, string commande = NULL);
		Trame();

		int16_t getTTL() const;
		void setTTL(const int16_t &value);
		string getCommande() const;
		void setCommande(const string &value);
};

#endif // TRAME_H
