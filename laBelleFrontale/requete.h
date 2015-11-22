#ifndef DEF_REQUETE
#define DEF_REQUETE

class Requete
{

	public:

	Requete();
	char* getAffectation();
	char* getStatut();
	char* getAction();
	char* getOption();
	char* getParametre();
	char* getGroupe();
	char* getCle();
	char* getRequete();
	char* getResultat();
	void decoupage(const char * chaine);
	void affichage();
	void construction();
	int tri(const char * resultat);

	private:

	char m_affectation[16];
	char m_statut[16];
	char m_action[16];
	char m_option[16];
	char m_parametre[16];
	char m_groupe[16];
	char m_cle[256];

	char m_requete[256];
	char m_resultat[256];

};

#endif
