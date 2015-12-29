#ifndef DEF_REQUETE
#define DEF_REQUETE

#include <string>
#include <iostream>
#include <sstream>   
#include <list>

class Requete
{
	private:

	std::string m_affectation;
	std::string m_statut;
	std::string  m_action;
	std::string m_option;
	std::string m_parametre;
	std::string m_nom;
	std::string m_partage;
	std::string m_groupe;
	std::string m_cle;
	bool pourBdd;

	std::string m_requete;
	std::string m_resultat;

	public:

	Requete();
	std::string getAffectation();
	std::string getStatut();
	std::string getAction();
	std::string getOption();
	std::string getParametre();
	std::string getGroupe();
	std::string getCle();
	std::string getRequete();
	std::string getResultat();
	bool getPourBdd();
	void setResultat(std::string buffer);
	int decoupage(std::string& chaine);
	void construction();
	int tri(std::list<std::string>& resultat);
};

#endif
