
#include "traitement_recu.h"


/*======================================================================
**  Nom          : traiter_recu
**  Description  : Permet d'effectuer les actions en fonction de ce que
**                 la requete reçu de la frontale
** ---------------------------------------------------------------------
**  Parametres   :
**      char * requete_recu : requete reçu par la frontale
**======================================================================*/

int traiter_recu (char * requete_recu)
{
	unsigned char requete_decrypt[sizeof(requete_recu)];
	puts("YIHI");
	decrypt(requete_recu, requete_decrypt, strlen(requete_recu));
	printf("JAI RECU -> %s\n", requete_recu);
	printf("DECRYPT -> %s\n", requete_decrypt);

	char * copie = malloc(sizeof (char) * 1024);

	strncpy(copie,requete_decrypt, 1024);
	copie[1023] = '\0';

	char *type = malloc (sizeof (char*) * 256);
	char *donnee = malloc (sizeof (char*) * 256);
	char * save_ptr;
	char *numero_requete = malloc (sizeof (char*) * 1024);
	printf("requete_decrypt -> %s",copie);
	numero_requete = strtok_r(copie, "*", &save_ptr);
	printf("requete_decrypt -> %s",copie);
	type = strtok_r(NULL, "*", &save_ptr);
        printf("requete_decrypt -> %s",copie);
	int j = 0;

	/*while (*save_ptr != '\0')	
	{
		donnee[j] = *(save_ptr++);
		j++;
	}*/

	//if (type[0] == 'R') printf("DONNEE de la reponse -> %s\n", donnee);


	switch(type[0])
	{
		case 'R' :{
				  while (*save_ptr != '\0')	
				  {
					donnee[j] = *(save_ptr++);
					j++;
				  }
				printf("DONNEE de la reponse -> %s\n", donnee);
				break;
		}
		case '1' :{
				

				char *status_requete = malloc (sizeof (char*) * 1024);
				
				char *affectation_requete = malloc (sizeof (char*) * 1024);
				char *test = malloc (sizeof (char*) * 1024);
				char *nom = recup_valeur("nom");
				printf("NOM -> %s\n", nom);
				//char *status = recup_valeur("status");
				printf("RECUP_STATUT -> %s\n", recup_valeur("status"));
				printf("RECUP_AFFECTATION -> %s\n", recup_valeur("affectation"));

				status_requete = strtok_r(NULL, "*", &save_ptr);
				affectation_requete = strtok_r(NULL, "*", &save_ptr);
				//test = strtok_r(NULL, "*", &save_ptr);
				//printf("TEST -> %s\n", test);
				printf("STATUS -> %s\n", status_requete);
				printf("AFFECTATION -> %s\n", affectation_requete);

				usleep(100);

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					sprintf(a_envoyer, "%s*1*%s*%s*EOF*", numero_requete, nom, recup_valeur("status"));
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					printf("a envoyer -> %s\n", a_envoyer);
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt, GIVE);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					strcat(a_envoyer,numero_requete);
					strcat(a_envoyer, "*1*none*none*EOF*");
					//sprintf(a_envoyer, "1*none");
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt, GIVE);	
				}
		break;
		}
		case '2' :{
				char *status_requete = malloc (sizeof (char) * 1024);
				char *affectation_requete = malloc (sizeof (char) * 1024);
				char *test = malloc (sizeof (char) * 1024);
				char *nom = recup_valeur("nom");
				printf("NOM -> %s\n", nom);
				//char *status = recup_valeur("status");
				printf("RECUP_STATUT -> %s\n", recup_valeur("status"));
				printf("RECUP_AFFECTATION -> %s\n", recup_valeur("affectation"));
				status_requete = strtok_r(NULL, "*", &save_ptr);
				affectation_requete = strtok_r(NULL, "*", &save_ptr);
				//test = strtok_r(NULL, "*", &save_ptr);
				//printf("TEST -> %s\n", test);
				printf("STATUT -> %s\n", status_requete);
				printf("AFFECTATION -> %s\n", affectation_requete);

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					sprintf(a_envoyer, "%s*2*%s*%s*EOF*",numero_requete, nom, recup_valeur("status"));
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt, GIVE);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					sprintf(a_envoyer, "%s*2*none*none*EOF*", numero_requete);
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt, GIVE);	
				}
		break;

		}
		case '3' :{
				char *status_requete = malloc (sizeof (char) * 128);//doit etre none
				char *affectation_requete = malloc (sizeof (char) * 128);
				char *groupe_requete = malloc (sizeof (char) * 128); //doit etre none
				char *datatype_requete = malloc (sizeof (char) * 128); 
				char *nom = recup_valeur("nom");
				printf("NOM -> %s\n", nom);
				printf("requete_decrypt -> %s",copie);
				//char *status = recup_valeur("status");
				printf("RECUP_STATUT -> %s\n", recup_valeur("status"));
				printf("RECUP_AFFECTATION -> %s\n", recup_valeur("affectation"));
				status_requete = strtok_r(NULL, "*", &save_ptr);
				affectation_requete = strtok_r(NULL, "*", &save_ptr);
				groupe_requete = strtok_r(NULL, "*", &save_ptr);
				datatype_requete = strtok_r(NULL, "*", &save_ptr);
				//test = strtok_r(NULL, "*", &save_ptr);
				//printf("TEST -> %s\n", test);
				//printf("STATUT -> %s\n", status_requete);
				printf("AFFECTATION -> %s\n", affectation_requete);
				printf("DATATYPE -> %s\n", datatype_requete);

				/*
				   Pour le test datatype, il faut juste savoir si on a une donnée 
				   correspondant au type voulu, stocké dans datatype_requete.
				   Je vous conseille de l'améliorer, mais le principe simple auquel je pense est qu'il faut 
				   inscrire les différents datatypes possibles dans un fichier de configuration,
				   puis, si un champ de type de donnée ne contient pas de référence, on met none.
				   Exemple : le client a une image : picture = refdel'image
				                   le client n'a pas d'image : picture = none
				    D'où le test ci-dessous :
				*/

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0 && strcmp(recup_valeur(datatype_requete), "none") != 0)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					sprintf(a_envoyer, "%s*3*%s;%s;%s;EOF*",numero_requete, recup_valeur("status"), recup_valeur(datatype_requete), nom);
					//En sortie de recup_valeur(datatype_request), on a la référence du document
					printf("a envoyer %s\n", a_envoyer);
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt, GIVE);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					sprintf(a_envoyer, "%s*3*none*none*none*EOF*", numero_requete);
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt, GIVE);	
				}
		break;

		}

	}
	return 0;

}
