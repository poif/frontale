#include "traitement_commande.h"
#include "default_lib.h"
int groupe_rep = 0;

/*======================================================================
**  Nom          : traiter_commande
**  Description  : Permet d'effectuer les actions en fonction de ce que
**                 l'utilisateur à taper dans le mini-shell
** ---------------------------------------------------------------------
**  Parametres   :
**      char * a_traiter : commande rentrée par l'utilisateur
**======================================================================*/

int traiter_commande(char *a_traiter)
{
	pthread_t attente_reponse;

	if(*a_traiter)
	{

		char *action = malloc (sizeof (char) * 256);
		char *option = malloc (sizeof (char) * 256);
		char *cible = malloc (sizeof (char) * 256);
		char *statut = malloc (sizeof (char) * 256);
		char *affectation = malloc (sizeof (char) * 256);

		if (memcmp(a_traiter, "exit", 4) == 0)
			return false;

		char *save_ptr;
		action = strtok_r(a_traiter, " ", &save_ptr);
		option = strtok_r(NULL, " ", &save_ptr);

		unsigned char a_envoyer[sizeof (char) * 1024];

		if (memcmp(action, "help", 4) == 0)
		{
			//if(option == NULL) //s'il n'y a pas d'option on affiche l'aide gÃ©nÃ©ral
				//afficher_aide();
			//else				//sinon on affiche l'option de la commande
			//	afficher_aide(option);
		}	

		else if (memcmp(action, "search", 6) == 0)
		{
			
			
			if(memcmp(option, "-n", 2) == 0){

				printf("On recherche le ou les noms d'une ou plusieurs personnes !\n");

				statut = strtok_r(NULL, " ", &save_ptr);
		
				int j = 0;

				/*while (*save_ptr != '\0')	
				{
					affectation[j] = *(save_ptr++);
					j++;
				}*/
				affectation = strtok_r(NULL, " ", &save_ptr);
				sprintf(a_envoyer, "chall*%s*%s*%s*%s*none*none*none", affectation, statut, 
					action, option);
			}
			else if(memcmp(option, "-e", 2) == 0){

				printf("On recherche l'existence d'une personne !\n");

				cible = strtok_r(NULL, " ", &save_ptr);
				statut = strtok_r(NULL, " ", &save_ptr);
		
				int j = 0;

				/*while (*save_ptr != '\0')	
				{
					affectation[j] = *(save_ptr++);
					j++;
				}*/
				affectation = strtok_r(NULL, " ", &save_ptr);

				sprintf(a_envoyer, "chall*%s*%s*%s*%s*%s*none*none",affectation, statut, 
					action, option, cible);
			}

			else if(memcmp(option, "-r", 2) == 0){

				char *nom = malloc (sizeof (char) * 256);
				char *groupe_cible = malloc (sizeof (char) * 256);

				printf("On recherche la reference d'un fichier !\n");

				cible = strtok_r(NULL, " ", &save_ptr);//type de fichier
				nom = strtok_r(NULL, " ", &save_ptr);

				statut = strtok_r(NULL, " ", &save_ptr);
				affectation = strtok_r(NULL, " ", &save_ptr);
				groupe_cible = strtok_r(NULL, " ", &save_ptr);
				char *mongroupe = malloc (sizeof (char) * 256);
				mongroupe = recup_groupe();
				sprintf(a_envoyer, "chall*%s*%s*%s*%s*%s*%s*%s*%s*%s*%s*none", affectation, statut, 
					action, option, cible, nom, recup_valeur("affectation"), recup_valeur("statut"), 
					groupe_cible, mongroupe);
			}

			printf("%s\n", a_envoyer);
			printf("SIZE -> %d\n", strlen(a_envoyer));
			unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
			memset(a_envoyer_crypt, '\0', sizeof(a_envoyer));
			unsigned char a_decrypt[sizeof(a_envoyer)];
			unsigned char tmp[sizeof(a_envoyer)];
			crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
			puts("AFTER");
			envoi_requete(a_envoyer_crypt, strlen(a_envoyer)+16-(strlen(a_envoyer)%16), WANT);
			//decrypt(a_envoyer_crypt, a_decrypt, sizeof(a_envoyer)+15);
			//printf("DECRYPT -> %s\n", a_decrypt);
			
		}

	else if (memcmp(action, "group", 5) == 0)
	{
		int j = 0;

		while (*save_ptr != '\0')	
		{
			cible[j] = *(save_ptr++);
			j++;
		}
		group_traitement(option, cible);
	}

	else if (memcmp(action, "share", 4) == 0)
	{
		puts("SHARE ENTER");
		char *name = strtok_r(NULL, " ", &save_ptr);
		char *path = strtok_r(NULL, " ", &save_ptr);
		char *type = strtok_r(NULL, " ", &save_ptr);
		char *groupe = strtok_r(NULL, " ", &save_ptr);
		share_traitement(option, name, path, type, groupe);
	}

	else if (memcmp(action, "delete", 6) == 0)
	{
		puts("DELETE");
		delete_traitement(option, cible);
	}

	else puts("Erreur de syntaxe");
}

	return true;
}

void group_traitement(char *op, char *name)
{
	unsigned char a_envoyer[sizeof (char*) * 1024];
	pthread_t attente_reponse;
	int rep;

	if(memcmp(op, "-join", 5) == 0)
	{
		sprintf(a_envoyer, "chall*none*none*group*none*%s*none*none", name);
		unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		unsigned char a_decrypt[sizeof(a_envoyer)];
		crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16,WANT);
		pthread_create(&attente_reponse, NULL, wait_answer);
		pthread_join(&attente_reponse, NULL);	

		if(groupe_rep == 1){
			create_group(name);
		}
		else 
		{
			printf("Le groupe n'existe pas, voulez-vous le creer? 0-> Non 1->Oui");
			scanf("%d", &rep);
			if (rep==1)
			{
				create_group(name);
			}
		}
		groupe_rep = 0;	

	}	

	else if (memcmp(op, "-create", 7) == 0)
	{
			sprintf(a_envoyer, "chall*none*none*group*none*%s*none*none", name);
			unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
			unsigned char a_decrypt[sizeof(a_envoyer)];
			crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
			envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16,WANT);
			pthread_create(&attente_reponse, NULL, wait_answer);
			pthread_join(&attente_reponse, NULL);
		
			if(groupe_rep == 0)
			{
				create_group(name);
			}
			else 
			{
				printf("Le groupe existe déjà, voulez-vous le rejoindre? 0-> Non 1->Oui");
				scanf("%d", &rep);
					if (rep==1){
						create_group(name);
					}
			}
			groupe_rep = 0;
		}
		else puts("Mauvais argument");
	
}

void share_traitement(char *op, char *name, char *path, char *type, char *group_name)
{
	unsigned char a_envoyer[sizeof (char*) * 1024];

	printf("NAME -> %s\n", name);

	printf("GROUPE -> %s\n", group_name);

	if (memcmp(op, "-local", 7) == 0) 
	{
		partage_create(name, path, 0, group_name);
	}
	else if (memcmp(op, "-bdd", 4) == 0)
	{
		/*char *yolo = malloc(1024*sizeof(char));
		retour_chaine_fic(path, yolo);
		puts("nia");
		printf("taille -> %i\n", strlen(yolo));
		printf("yolo -> %s\n", yolo);
		puts("YOLO");*/
		printf("ce qu'on veut -> %s\n", retour_chaine_fic(path));
		sprintf(a_envoyer, "chall*%s*%s*insert*%s*%s*%s*+$;+$;*%s*none", recup_valeur("affectation"), 
			recup_valeur("statut"), type, retour_chaine_fic(path), name, group_name);
		printf("j'envois -> %s\n\n", a_envoyer);
		puts("HEY");
		unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		unsigned char a_decrypt[sizeof(a_envoyer)];
		crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16,WANT);
		puts("hum");
		partage_create(name, path, type, 1, group_name);
		puts("partage");
	}
	else puts("Mauvais argument");

}
void delete_traitement(char *op, char *name/*, char *path*/)
{
	unsigned char a_envoyer[sizeof (char*) * 1024];


	if (memcmp(op, "-local", 7) == 0)
	{
			printf("NAME -> %s\n", name);
		printf("OP -> %s\n", op);
		partage_delete(strtok(name, " "));
	}

	else if (memcmp(op, "-bdd", 4) == 0)
	{
		sprintf(a_envoyer, "chall*%s*%s*delete*none*%s*%s*%s", recup_valeur("affectation"), 
			recup_valeur("statut"), name, recup_valeur("nom"), recup_groupe());
		unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		unsigned char a_decrypt[sizeof(a_envoyer)];
		crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16,WANT);
		partage_delete(name);
	}

	else puts("Mauvais argument");

}
