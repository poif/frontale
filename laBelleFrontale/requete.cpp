#include "requete.h"
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

using namespace std;

Requete::Requete()
{
	m_affectation[0] = '\0';
	m_statut[0] = '\0';
	m_action[0] = '\0';
	m_option[0] = '\0';
	m_parametre[0] = '\0';
	m_groupe[0] = '\0';
	m_cle[0] = '\0';

	m_requete[0] = '\0';
	m_resultat[0] = '\0';
}

//Getters

char* Requete::getAffectation()
{	return m_affectation;}
char* Requete::getStatut()
{	return m_statut;}
char* Requete::getAction()
{       return m_action;}
char* Requete::getOption()
{       return m_option;}
char* Requete::getParametre()
{       return m_parametre;}
char* Requete::getGroupe()
{       return m_groupe;}
char* Requete::getCle()
{       return m_cle;}
char* Requete::getRequete()
{	return m_requete;}
char* Requete::getResultat()
{	return m_resultat;}

//Methods

int Requete::tri(const char *resultat) //tri les resultats recu et garde les éléments nécessaire suivant le type de requête
{

	int cpt_resultat=0;
	int cpt_element=0;
	int cpt_name=0;
	char sep = '*';
	char hash_recu[512];
	char name[512];
	char reference[512];


	SHA_CTX ctx;
        char hash[SHA_DIGEST_LENGTH];
	SHA1_Init(&ctx);



	if(strcmp(m_action,"search") == 0)
	{
		if(strcmp(m_option,"-n") == 0) //Si on recherche le nom de qqn
		{
			SHA1_Update(&ctx,m_statut,strlen(m_statut));
			SHA1_Final((unsigned char*)hash,&ctx);
			
			do //on parcours toute la requete recu
			{
				while(resultat[cpt_resultat] != sep) //on récupère le nom
				{
					name[cpt_element]=resultat[cpt_resultat];
					cpt_resultat++;
					cpt_element++;
				}
				name[cpt_element]='\0';
				cpt_resultat++;
				cpt_element=0;

				while(resultat[cpt_resultat] && resultat[cpt_resultat] != sep) //on récupère le hash du statut correspondant au nom
				{
					hash_recu[cpt_element]=resultat[cpt_resultat];
					cpt_resultat++;
					cpt_element++;
				}
				//hash_recu[cpt_element]='\0';
				hash_recu[SHA_DIGEST_LENGTH] = '\0';

				cpt_element=0;
				cpt_resultat++;

				if(strcmp(hash,hash_recu) == 0) //si les hashs sont égaux alors on rajoute le nom à la liste
				{

					while(name[cpt_element] != '\0')
					{
						m_resultat[cpt_name]=name[cpt_element];
						cpt_name++;
						cpt_element++;

					}

					m_resultat[cpt_name]='\0';
					cpt_name++;
					cpt_element=0;
				}

			}while(resultat[cpt_resultat] != '\0');
			m_resultat[cpt_name]='\0';
		}

		else if (strcmp(m_option,"-e") ==0)
		{
			SHA1_Update(&ctx,m_parametre,strlen(m_parametre));
                        SHA1_Final((unsigned char*)hash,&ctx);

			do
			{
				while(resultat[cpt_resultat] != sep)
				{
					hash_recu[cpt_element]=resultat[cpt_resultat];
					cpt_element++;
					cpt_resultat++;
				}
				hash_recu[cpt_element]='\0';
				cpt_element=0;
				cpt_resultat++;

				if(strcmp(hash_recu,hash) == 0)
				{
					m_resultat[0] = 'y';
					m_resultat[1] = 'e';
					m_resultat[2] = 's';
					m_resultat[3] = '\0';
					return 1;
				}
			}while(resultat[cpt_resultat] != '\0');

			m_resultat[0] = 'n';
			m_resultat[1] = 'o';
			m_resultat[2] = '\0';
		}

		else if (strcmp(m_option,"-p") ==0)
		{
			SHA1_Update(&ctx,m_parametre,strlen(m_parametre));
                        SHA1_Final((unsigned char*)hash,&ctx);

			do
			{
				while(resultat[cpt_resultat] != sep)
				{
					reference[cpt_element]=resultat[cpt_resultat];
					cpt_resultat++;
					cpt_element++;
				}
				reference[cpt_element]='\0';
				cpt_resultat++;
				cpt_element=0;

				while(resultat[cpt_resultat] != sep)
				{
					hash_recu[cpt_element]=resultat[cpt_resultat];
					cpt_resultat++;
					cpt_element++;
				}
				hash_recu[cpt_element]='\0';
				cpt_element=0;
				cpt_resultat++;

				if(strcmp(hash,hash_recu) == 0)
				{
					while(reference[cpt_element] != '\0')
					{
						m_resultat[cpt_name]=reference[cpt_element];
						cpt_element++;
						cpt_name++;
					}
					m_resultat[cpt_name]=sep;
					cpt_name++;
					cpt_element=0;

					while(m_groupe[cpt_element] != '\0')
					{
						m_resultat[cpt_name]=m_groupe[cpt_element];
						cpt_name++;
						cpt_element++;
					}
					m_resultat[cpt_name]='\0';
					return 1;
				}
			}while(resultat[cpt_resultat] != '\0');
		}
		else
		{
			printf("Tri : Option inconnu\n");
			return 0;
		}
	}
	else
	{
		printf("Tri : Action inconnu\n");
		return 0;
	}
	return 1;
}

void Requete::construction() //construit la requete suivant action, option et parametre
{
	int i=0;
	int j=0;
	char sep = '*';

	if(strcmp(m_action,"search") == 0)
	{
		if(strcmp(m_option,"-n") == 0)
		{
		while(m_affectation[i] != '\0')
			{
				m_requete[i]=m_affectation[i];
				i++;
			}
			m_requete[i]='\0';
		}

		else if(strcmp(m_option,"-e") == 0)
		{
			while(m_statut[i] != '\0')
			{
				m_requete[i]=m_statut[i];
				i++;
			}
			m_requete[i]='\0';
		}

		else if(strcmp(m_option,"-p") == 0)
		{
			while(m_option[i] != '\0')
			{
				m_requete[i]=m_option[i];
				i++;
			}
			m_requete[i]=sep;
			i++;

			while(m_statut[j] != '\0')
			{
				m_requete[i]=m_statut[j];
				j++;
				i++;
			}
			m_requete[i]='\0';
		}

		else
			printf("option inconnue\n");
	}
	else
		printf("action inconnue\n");
}

void Requete::affichage()
{
	printf(" statut : %s\n affectation : %s\n action : %s\n option : %s\n parametre : %s\n groupe : %s\n cle : %s\n",m_statut,m_affectation,m_action,m_option,m_parametre,m_groupe,m_cle);
}

int Requete::test_char(char caractere)
{
	if(caractere < 127 && caractere > 31)
		return 1;
	else
		return 0;
}

int Requete::decoupage(const char * chaine)
{
	int test;
	int cpt_chaine = 0;
	int cpt_element = 0;
	char sep = '*'; //on considère ici que '*' est le séparateur

	while(chaine[cpt_chaine] != sep)
	{
		test = test_char(chaine[cpt_chaine]);
		if(test == 0)
		{
			printf("Requete malformée\n");
			return 0;
		}

		m_affectation[cpt_element]=chaine[cpt_chaine]; //on met les caracteres 1 à 1 de la chaine dans l'élément jusqu'au séparateur
		cpt_chaine ++;
		cpt_element ++;
	}
	m_affectation[cpt_element] = '\0'; //pour indiquer la fin de l'élément
	cpt_element = 0;
	cpt_chaine++;

	while(chaine[cpt_chaine] != sep)
        {
                test = test_char(chaine[cpt_chaine]);
                if(test == 0)
                {
                        printf("Requete malformée\n");
                        return 0;
                }

		m_statut[cpt_element]=chaine[cpt_chaine];
                cpt_chaine ++;
                cpt_element ++;
        }
        m_statut[cpt_element] = '\0';
	cpt_element = 0;
	cpt_chaine++;

	while(chaine[cpt_chaine] != sep)
        {
                test = test_char(chaine[cpt_chaine]);
                if(test == 0)
                {
                        printf("Requete malformée\n");
                        return 0;
                }

		m_action[cpt_element]=chaine[cpt_chaine];
                cpt_chaine ++;
                cpt_element ++;
        }
        m_action[cpt_element] = '\0';
	cpt_element = 0;
	cpt_chaine++;

	while(chaine[cpt_chaine] != sep)
        {
		test = test_char(chaine[cpt_chaine]);
                if(test == 0)
                {
                        printf("Requete malformée\n");
                        return 0;
                }

                m_option[cpt_element]=chaine[cpt_chaine];
                cpt_chaine ++;
                cpt_element ++;
        }
        m_option[cpt_element] = '\0';
	cpt_element = 0;
	cpt_chaine++;

	while(chaine[cpt_chaine] != sep)
        {
                test = test_char(chaine[cpt_chaine]);
                if(test == 0)
                {
                        printf("Requete malformée\n");
                        return 0;
                }

		m_parametre[cpt_element]=chaine[cpt_chaine];
                cpt_chaine ++;
                cpt_element ++;
        }
        m_parametre[cpt_element] = '\0';
	cpt_element = 0;
	cpt_chaine++;

	while(chaine[cpt_chaine] != sep)
        {
                test = test_char(chaine[cpt_chaine]);
                if(test == 0)
                {
                        printf("Requete malformée\n");
                        return 0;
                }

		m_groupe[cpt_element]=chaine[cpt_chaine];
                cpt_chaine ++;
                cpt_element ++;
        }
        m_groupe[cpt_element] = '\0';
	cpt_element = 0;
	cpt_chaine++;

	while(chaine[cpt_chaine] != '\0') //ici fin de la chaine (donc pas de séparateur)
        {
                test = test_char(chaine[cpt_chaine]);
                if(test == 0)
                {
                        printf("Requete malformée\n");
                        return 0;
                }

		m_cle[cpt_element]=chaine[cpt_chaine];
                cpt_chaine ++;
                cpt_element ++;
        }
	m_cle[cpt_element] = '\0';
	return 1;
}




