#ifndef DEF_REQUETE
#define DEF_REQUETE

class Requete
{
	private:

	char m_affectation[64];
	char m_statut[64];
	char m_action[64];
	char m_option[64];
	char m_parametre[64];
	char m_groupe[64];
	char m_cle[256];

	char m_requete[256];
	char m_resultat[256];
        bool pourBdd;


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
	bool getPourBdd();
	int test_char(char caractere);
	int decoupage(const char * chaine);
	void affichage();
	void construction();
	int tri(const char * resultat);
};

#endif
