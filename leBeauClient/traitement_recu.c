#include "traitement_recu.h"

static int flag = 0;//flag pour le wait

extern int groupe_rep;

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

    char *type = malloc (sizeof (char) * 256);
    char *donnee = malloc (sizeof (char) * 256);
    char * save_ptr;
    char *numero_requete = malloc (sizeof (char) * 1024);

    char * copie = malloc(sizeof (char) * 1024);

    strncpy(copie,requete_decrypt, 1024);
    copie[1023] = '\0';

    numero_requete = strtok_r(copie, "*", &save_ptr);


    if(numero_requete[0] == 'R')
    {
    	type = 'z';
    	traitement_R(copie);
    }

    else
	    type = strtok_r(NULL, "*", &save_ptr);

	int j = 0;


	switch(type[0])
	{
				
		case '1' :				
		{
				char *status_requete = malloc (sizeof (char) * 1024);
				
				char *affectation_requete = malloc (sizeof (char) * 1024);
				//char *test = malloc (sizeof (char) * 1024);
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
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*1*%s*%s*EOF", numero_requete, nom, recup_valeur("status"));
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					printf("a envoyer -> %s\n", a_envoyer);
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char) * 1024];
					strcat(a_envoyer,numero_requete);
					strcat(a_envoyer, "chall*1*none*EOF");
					//sprintf(a_envoyer, "1*none");
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);	
				}

				break;
		}

		case '2' :
		{
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
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*2*%s*%s*EOF",numero_requete, nom, recup_valeur("status"));
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*2*none*EOF", numero_requete);
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);	
				}
				break;
		}

		case '3' :
		{
				char *status_requete = malloc (sizeof (char) * 1024);//doit etre none
				char *affectation_requete = malloc (sizeof (char) * 1024);
				char *groupe_requete = malloc (sizeof (char) * 1024); //doit etre none
				char *datatype_requete = malloc (sizeof (char) * 1024); 
				char *test = malloc (sizeof (char) * 1024);
				char *nom = recup_valeur("nom");
				printf("NOM -> %s\n", nom);
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

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0 && partage_demande(datatype_requete, S_TYPE) == 1)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char) * 1024];
					//sprintf(a_envoyer, "%s*3*%s*%s*%s*EOF",numero_requete, recup_valeur("status"), recup_valeur(datatype_requete), nom);
					sprintf(a_envoyer, "chall*%s*3*%s;%s*EOF",numero_requete, retour_reference(datatype_requete), nom);
					//En sortie de recup_valeur(datatype_request), on a la référence du document
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*3*none*EOF", numero_requete);
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt);	
				}
				break;
			
		}

				case '4' :
				{
					unsigned char a_envoyer[sizeof (char) * 1024];
					char *groupe_demande = malloc (sizeof (char) * 1024);
					char *ref = malloc (sizeof (char) * 1024);
					
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					ref = strtok_r(NULL, "*", &save_ptr);
					ref = strtok_r(NULL, "*", &save_ptr);


					if (groupe_correspondance(groupe_demande))
					{
						envoi_fichier(retour_path(ref), numero_requete);
					}
					else
					{
						unsigned char a_envoyer[sizeof (char) * 1024];
						sprintf(a_envoyer, "chall*%s*4*none*EOF", numero_requete);
						unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
						crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
						envoi_requete(a_envoyer_crypt);	

					}

					break;
			
				}

				case '6' :
				{
					char *groupe_demande = malloc (sizeof (char) * 1024);
					unsigned char a_envoyer[sizeof (char) * 1024];
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					if (groupe_correspondance(groupe_demande) == 1)
					{
						sprintf(a_envoyer, "chall*%s*6*1*EOF", numero_requete);
					}
					else
					{
						sprintf(a_envoyer, "chall*%s*6*0*EOF", numero_requete);
					}
				}

	}
	return 0;

}

void traitement_R(char *recu)
{
	char *save_ptr;
	char *numero = malloc (sizeof (char) * 1024);

	numero = strtok_r(recu, "R*", &save_ptr);

	switch(numero[0])
	{
		case '1':
		{
			;
			int i = 0;
			while(recu[i] != '\0')
			{
				if(recu[i] == 'R');
				else if(recu[i] == '*')printf("\n");
				else printf("%c", recu[i]);
				i++;
			}
		}

		case '2':
		{
			if(recu[4] == 1)
				puts("OUI");
			else
				puts("NON");
		}

		case '4':
		{
			char *fichier= malloc (sizeof (char) * 1024);
			fichier = strtok(recu, "R*4*");

			if (strcmp(fichier, "none") == 0) puts("Aucun fichier");
			else recevoir_fichier(fichier);
		}

		case '6':
		{
			if(recu[4] == 1)
				puts("Groupe déjà existant !");
			else
			{
				puts("Groupe");
				groupe_rep = 1;
			}

			flag = 1;
		}
	}
}

void recevoir_fichier(char *fichier)
{
	 FILE * recvFILE;
	 recvFILE = fopen ( "newdl","w+" );
	 fwrite (fichier, sizeof(fichier) , 1024, recvFILE );
	 fclose(recvFILE);
}

void *wait_answer()
{
	while(flag ==0 );
	
	flag = 0;
}
