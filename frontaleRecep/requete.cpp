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
	m_nom[0] = '\0';
	m_partage[0] = '\0';
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
bool Requete::getPourBdd()
{	return pourBdd;}

void Requete::setResultat(const char * buffer)
{	strncpy(m_resultat, buffer, 63);}

//Methods

/* =======================================================================================================================
	FONCTION TRI : TRI ET CONSTRUIT LE RESULTAT A ENVOYER AU CLIENT
=========================================================================================================================*/

int Requete::tri(const char *resultat) //tri les resultats recu et garde les éléments nécessaire suivant le type de requête => construit la requete à envoyer sur le réseau
{
	int cpt_resultat=0;
	int cpt_element=0;
	int cpt_name=0;
	char sep = '*';
	char hash_recu[512];
	char name[512];
	char reference[512];

	SHA_CTX ctx;
        	char hash[SHA_DIGEST_LENGTH+1];
	SHA1_Init(&ctx);

	if(strcmp(m_action,"search") == 0)
	{
		if(strcmp(m_option,"-n") == 0) //Si on recherche le nom de qqn
		{
			SHA1_Update(&ctx,m_statut,strlen(m_statut));
			SHA1_Final((unsigned char*)hash,&ctx);
			hash[SHA_DIGEST_LENGTH] = '\0';

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

				while(cpt_element < SHA_DIGEST_LENGTH && resultat[cpt_resultat] != sep) //on récupère le hash du statut correspondant au nom recu depuis la requete
				{
					hash_recu[cpt_element]=resultat[cpt_resultat];
					cpt_resultat++;
					cpt_element++;
				}
				hash_recu[cpt_element]='\0';
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
					m_resultat[cpt_name]=sep;
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

	else if(strcmp(m_action, "insert")==0 || strcmp(m_action, "seek")==0 || strcmp(m_action, "delete")==0 || strcmp(m_action, "select")==0) // Si c'était une interaction bdd, il faut juste retransmettre le message au client
	{
		while(resultat[cpt_resultat] != '\0')
		{
			m_resultat[cpt_resultat] = resultat[cpt_resultat]; // Copie du resultat
			cpt_resultat ++;
		}
		m_resultat[cpt_resultat] = '\0';
	}

	else
	{
		printf("Tri : Action inconnu\n");
		return 0;
	}
	return 1;
}

/*=========================================================================================
	FONCTION DE CONSTRUCTION : CONSTRUIT LA REQUETE A ENVOYER (AU RESEAU OU A LA BDD)
=========================================================================================*/

void Requete::construction() //construit la requete suivant action, option et parametre
{
	int i=0;
	int j=0;
	char sep = '*';
	SHA_CTX ctx;
        char hash[SHA_DIGEST_LENGTH];
        SHA1_Init(&ctx); 		//Initialisation pour calculer le hash

	if(strcmp(m_action,"search") == 0) // Fonction recherche
	{
		pourBdd=false;
		if(strcmp(m_option,"-n") == 0) // Si on cherche un nom
		{
		while(m_affectation[i] != '\0')
			{
				m_requete[i]=m_affectation[i];  //La requete à envoyer est composé uniquement de l'affectation
				i++;
			}
			m_requete[i]='\0';
		}

		else if(strcmp(m_option,"-e") == 0) // Si on cherche l'existance
		{
			while(m_statut[i] != '\0')
			{
				m_requete[i]=m_affectation[i]; // La requete à envoyer est composé uniquement du statut
				i++;
			}
			m_requete[i]='\0';
		}

		else if(strcmp(m_option,"-p") == 0) // Si on cherche une photo(donnée)
		{
			while(m_option[i] != '\0')
			{
				m_requete[i]=m_option[i]; // Premiere partie de la requete : l'option de la requete
				i++;
			}
			m_requete[i]=sep;
			i++;

			while(m_statut[j] != '\0')
			{
				m_requete[i]=m_statut[j]; // Deuxieme partie : le statut
				j++;
				i++;
			}
			m_requete[i]='\0';
		}

		else
			printf("option inconnue\n");
	}
	else if(strcmp(m_action,"insert") == 0) // Cas d'ajout d'une donneé dans la bdd
	{
		SHA1_Update(&ctx,m_nom,strlen(m_nom));
	        SHA1_Final((unsigned char*)hash,&ctx); //calcul du hash et stockage dans hash
	    pourBdd=true;
		m_requete[0] = '3';  // Premiere partie : l'action (insert : 302)
		m_requete[1] = '0';
		m_requete[2] = '2';
		m_requete[3] = '*';
		i=4;

		while(m_statut[j] != '\0')
		{
			m_requete[i] = m_statut[j]; // Deuxieme partie : statut
			i++;
			j++;
		}
		m_requete[i] = sep; // On met un séparateur
		i++;
		j=0;

		while(m_affectation[j] != '\0')
		{
			m_requete[i] = m_affectation[j]; // 3e partie : l'affectation
			i++;
			j++;
		}
		m_requete[i] = sep; // On met un séparateur
		i++;
		j=0;

		while(m_groupe[j] != '\0')
		{
			m_requete[i] = m_groupe[j]; // 4e partie : le groupe
			i++;
			j++;
		}
		m_requete[i] = sep; // On met un séparateur
		i++;
		j=0;

		while(m_option[j] != '\0')
		{
			m_requete[i] = m_option[j]; // 5e partie : l'option (le type de la donnée)
			i++;
			j++;
		}
		m_requete[i] = sep; // On met un séparateur
		i++;
		j=0;

		while(m_parametre[j] != '\0')
		{
			m_requete[i] = m_parametre[j]; // 6e partie : la donnée en question
			i++;
			j++;
		}
		m_requete[i] = sep; // On met un séparateur
		i++;
		j=0;

		while(hash[j] != '\0')
		{
			m_requete[i] = hash[j]; // 7e partie : le hash du nom
			i++;
			j++;
		}
		m_requete[i] = sep;
		i++;
		j=0;

		m_requete[i] = 'E';
		m_requete[i+1] = 'O';	// 8e et dernière partie : EOF
		m_requete[i+2] = 'F';
		m_requete[i+3] = '\0'; // Fin de la requete
	}

	else if(strcmp(m_action,"delete")==0)
	{
				pourBdd=true;
                m_requete[0] = '3';  // Premiere partie : l'action (delete : 303)
                m_requete[1] = '0';
                m_requete[2] = '3';
                m_requete[3] = '*';
		i=4;

		while(m_parametre[j] != '\0')
		{
			m_requete[i] = m_parametre[j];	//2e partie : la référence (stocker dans m_parametre)
			i++;
			j++;
		}
		m_requete[i] = sep;
		i++;
		j=0;

		SHA1_Update(&ctx, m_nom, strlen(m_nom));
		SHA1_Final((unsigned char*)hash, &ctx);	//hash reçoit le hash du nom

		while(hash[j] != '\0')
		{
			m_requete[i] = hash[j]; // 3e partie : le hash du nom.
			i++;
			j++;
		}
		m_requete[i] = sep;
		i++;
		j=0;

		m_requete[i] = 'E';
		m_requete[i+1] = 'O';
		m_requete[i+2] = 'F';
		m_requete[i+3] = '\0'; //3e partie : EOF
	}

	else
		printf("action inconnue\n");
}

/*============================================================================================
	FONCTION AFFICHAGE : INUTILE DANS LE PROJET (utilisée pour des tests)
============================================================================================*/

void Requete::affichage() //Fonction inutile dans la frontale (je l'utilise pour mes tests)
{
	printf(" statut : %s\n affectation : %s\n action : %s\n option : %s\n parametre : %s\n nom : %s\n politique : %s\n groupe : %s\n cle : %s\n",m_statut,m_affectation,m_action,m_option,m_parametre,m_nom,m_partage,m_groupe,m_cle);
}

/*===========================================================================================
	FONCTION DE TEST DES CARACTERES
===========================================================================================*/

int Requete::test_char(char caractere) //test si le caractère est bien une lettre minuscule
{
	if(caractere < 127 && caractere > 31)
		return 1;
	else
		return 0;
}

/*==========================================================================================
	FONCTION DE REMISE À ZERO DES VARIABLES
===========================================================================================*/

void Requete::raz()
{
	m_affectation[0] = '\0';
        m_statut[0] = '\0';
        m_action[0] = '\0';
        m_option[0] = '\0';
        m_parametre[0] = '\0';
        m_nom[0] = '\0';
        m_partage[0] = '\0';
        m_groupe[0] = '\0';
        m_cle[0] = '\0';

        m_requete[0] = '\0';
        m_resultat[0] = '\0';
}

/*===========================================================================================
	FONCTION DE DECOUPAGE : DECOUPE LA REQUETE RECU DU CLIENT (1ere fonction appelée)
===========================================================================================*/

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

	if(strcmp(m_action,"insert") == 0 || strcmp(m_action,"delete") == 0) // Si c'est une requête pour la bdd on a un champ en plus : le nom de la personne
	{
		while(chaine[cpt_chaine] != sep)
		{
			test = test_char(chaine[cpt_chaine]);
			if(test == 0)
			{
				printf("Requete malformée\n");
				return 0;
			}

			m_nom[cpt_element] = chaine[cpt_chaine];
			cpt_chaine ++;
			cpt_element ++;
		}
		m_nom[cpt_element] = '\0';
		cpt_element = 0;
		cpt_chaine++;

		if(strcmp(m_action,"insert") == 0)	// Et si la requete est insert => champ supplémentaire : politique de partage
		{
			while(chaine[cpt_chaine] != sep)
			{
				test = test_char(chaine[cpt_chaine]);
                        	if(test == 0)
                        	{
                                	printf("Requete malformée\n");
                                	return 0;
                        	}

				m_partage[cpt_element] = chaine[cpt_chaine];
				cpt_chaine++;
				cpt_element++;
			}
			m_partage[cpt_element] = '\0';
			cpt_element = 0;
			cpt_chaine ++;
		}
	}

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




