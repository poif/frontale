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
	char m_nom[64];
	char m_partage[64];
	char m_groupe[64];
	char m_cle[256];
	bool pourBdd;

	char m_requete[256];
	char m_resultat[256];

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

	void setResultat(const char * buffer);

	int test_char(char caractere);
	int decoupage(const char * chaine);
	void affichage();
	void construction();
	void raz();
	int tri(const char * resultat);
};

#endif
