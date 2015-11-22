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
	char *action = malloc (sizeof (char*) * 256);
	char *option = malloc (sizeof (char*) * 256);
	char *cible = malloc (sizeof (char*) * 256);

	if (memcmp(a_traiter, "exit", 4) == 0)
		return false;

	char *save_ptr;
	action = strtok_r(a_traiter, " ", &save_ptr);
	option = strtok_r(NULL, " ", &save_ptr);
	//cible = strtok_r(NULL, " ", &save_ptr);
	int j = 0;

	puts("YOLO");

	while (*save_ptr != '\0')	
	{
		cible[j] = *(save_ptr++);
		j++;
	} 

	printf("%s\n", cible);

	char *a_envoyer = malloc (sizeof (char*) * 1024);

	if (memcmp(action, "help", 4) == 0)
	{
		//if(option == NULL) //s'il n'y a pas d'option on affiche l'aide général
			//afficher_aide();
		//else				//sinon on affiche l'option de la commande
		//	afficher_aide(option);
	}	

	else if (memcmp(action, "search", 6) == 0)
	{
		sprintf(a_envoyer, "none*none*%s*%s*%s*none*none", action, option, cible);
		printf("%s\n", a_envoyer);
		unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
		crypt(a_envoyer, a_envoyer_crypt);
		printf("%s\n", a_envoyer_crypt);
		puts("AFTER");
		envoi_requete(a_envoyer_crypt);
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

	return true;
}