#include "traitement_commande.h"

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
	if(*a_traiter)
	{

		char *action = malloc (sizeof (char*) * 256);
		char *option = malloc (sizeof (char*) * 256);
		char *cible = malloc (sizeof (char*) * 256);
		char *statut = malloc (sizeof (char*) * 256);
		char *affectation = malloc (sizeof (char*) * 256);

		if (memcmp(a_traiter, "exit", 4) == 0)
			return false;

		char *save_ptr;
		action = strtok_r(a_traiter, " ", &save_ptr);

		unsigned char a_envoyer[sizeof (char*) * 1024];

		if (memcmp(action, "help", 4) == 0)
		{
			//if(option == NULL) //s'il n'y a pas d'option on affiche l'aide général
				//afficher_aide();
			//else				//sinon on affiche l'option de la commande
			//	afficher_aide(option);
		}	

		else if (memcmp(action, "search", 6) == 0)
		{
			option = strtok_r(NULL, " ", &save_ptr);

			if(memcmp(option, "-n", 2) == 0){

				printf("On recherche le ou les noms d'une ou plusieurs personnes !\n");

				statut = strtok_r(NULL, " ", &save_ptr);
		
				int j = 0;

				while (*save_ptr != '\0')	
				{
					affectation[j] = *(save_ptr++);
					j++;
				}
				sprintf(a_envoyer, "%s*%s*%s*%s*none*none*none", affectation, statut, action, option);
			}
			else if(memcmp(option, "-e", 2) == 0){

				printf("On recherche l'existence d'une personne !\n");

				cible = strtok_r(NULL, " ", &save_ptr);
				statut = strtok_r(NULL, " ", &save_ptr);
		
				int j = 0;

				while (*save_ptr != '\0')	
				{
					affectation[j] = *(save_ptr++);
					j++;
				}

				sprintf(a_envoyer, "%s*%s*%s*%s*%s*none*none", affectation, statut, action, option, cible);
			}
			else if(memcmp(option, "-p",2) == 0) {

				printf("On recherche la reference d'une photo !\n");

				cible = strtok_r(NULL, " ", &save_ptr);
				statut = strtok_r(NULL, " ", &save_ptr);
		
				int j = 0;

				while (*save_ptr != '\0')	
				{
					affectation[j] = *(save_ptr++);
					j++;
				}

				sprintf(a_envoyer, "%s*%s*%s*%s*%s*none*none", affectation, statut, action, option, cible);
			}
			printf("%s\n", a_envoyer);
			printf("SIZE -> %d\n", strlen(a_envoyer));
			unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
			unsigned char a_decrypt[sizeof(a_envoyer)];
			unsigned char tmp[sizeof(a_envoyer)];
			crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
			printf("%s\n", a_envoyer_crypt);
			puts("AFTER");
			envoi_requete(a_envoyer_crypt);
			decrypt(a_envoyer_crypt, a_decrypt, strlen(a_envoyer_crypt));
			printf("DECRYPT -> %s\n", a_decrypt);
		}

		else if (memcmp(action, "group", 5) == 0)
		{
			puts("GROUP ENTER");
		}

		else if (memcmp(action, "share", 4) == 0)
		{
			puts("SHARE ENTER");
		}

		else if (memcmp(action, "delete", 6) == 0)
		{
			puts("DELETE ENTER");
		}

		else puts("Erreur de syntaxe");
	}
	
	return true;
}

void group_traitement(char *op, char *name)
{
	unsigned char a_envoyer[sizeof (char*) * 1024];

	if (memcmp(op, "-join", 5) || memcmp(op, "-create", 7) == 0)
	{
		sprintf(a_envoyer, "none*none*group*none*%s*none*none", name);
		unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		unsigned char a_decrypt[sizeof(a_envoyer)];
		crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		envoi_requete(a_envoyer_crypt);
		//traiter retour frontale gestionnaire signal ?
		create_group(name);
	}

	else puts("Mauvais argument");
}

void share_traitement(char *op, char *name, char *path, char *group_name)
{
	unsigned char a_envoyer[sizeof (char*) * 1024];

	if (memcmp(op, "-local", 7))
	{
		//sprintf(a_envoyer, "none*share*%s*%s*0*%s*none", name, path, group_name);
		//unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		//unsigned char a_decrypt[sizeof(a_envoyer)];
		//crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		//envoi_requete(a_envoyer_crypt);
		partage_create(name, path, 0, group_name);
	}
	if (memcmp(op, "-bdd", 4))
	{
		sprintf(a_envoyer, "none*share*%s*%s*1*%s*none", name, path, group_name);
		unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		unsigned char a_decrypt[sizeof(a_envoyer)];
		crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		envoi_requete(a_envoyer_crypt);
		//traiter retour frontale gestionnaire signal ?
		partage_create(name, path, 1, group_name);
	}
	else puts("Mauvais argument");

}
void delete_traitement(char *op, char *name, char *path)
{
	unsigned char a_envoyer[sizeof (char*) * 1024];

	if (memcmp(op, "-local", 7))
	{
		//sprintf(a_envoyer, "none*none*delete*%s*0*%s*none", name, path);
		//unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		//unsigned char a_decrypt[sizeof(a_envoyer)];
		//crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		//envoi_requete(a_envoyer_crypt);
		partage_delete(name, path);
	}

	if (memcmp(op, "-bdd", 4))
	{
		sprintf(a_envoyer, "none*none*delete*%s*1*%s*none", name, path);
		unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		unsigned char a_decrypt[sizeof(a_envoyer)];
		crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
		envoi_requete(a_envoyer_crypt);
		//traiter retour frontale gestionnaire signal ?
		partage_delete(name, path);
	}

	else puts("Mauvais argument");

}