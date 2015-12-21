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

	char *type = malloc (sizeof (char*) * 256);
	char *donnee = malloc (sizeof (char*) * 256);
	char * save_ptr;

	type = strtok_r(requete_decrypt, "*", &save_ptr);

	int j = 0;

	/*while (*save_ptr != '\0')	
	{
		donnee[j] = *(save_ptr++);
		j++;
	}*/

	//if (type[0] == 'R') printf("DONNEE de la reponse -> %s\n", donnee);


	switch(type[0])
	{
		case 'R' :
				  while (*save_ptr != '\0')	
				  {
					donnee[j] = *(save_ptr++);
					j++;
				  }
				printf("DONNEE de la reponse -> %s\n", donnee);
				break;

		case '1' :
				;

				char *status_requete = malloc (sizeof (char*) * 1024);
				char *affectation_requete = malloc (sizeof (char*) * 1024);
				char *test = malloc (sizeof (char*) * 1024);
				char *nom = recup_valeur("nom");
				printf("NOM -> %s\n", nom);
				//char *status = recup_valeur("status");
				printf("RECUP_STATUS -> %s\n", recup_valeur("status"));
				printf("RECUP_STATUS -> %s\n", recup_valeur("affectation"));
				status_requete = strtok_r(NULL, "*", &save_ptr);
				affectation_requete = strtok_r(NULL, "*", &save_ptr);
				//test = strtok_r(NULL, "*", &save_ptr);
				//printf("TEST -> %s\n", test);
				printf("STATUS -> %s\n", status_requete);
				printf("AFFECTATION -> %s\n", affectation_requete);

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					sprintf(a_envoyer, "1*%s*%s", nom, recup_valeur("status"));
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char *) * 1024];
					sprintf(a_envoyer, "1*none");
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);	
				}
				break;

	}
	return 0;

}
