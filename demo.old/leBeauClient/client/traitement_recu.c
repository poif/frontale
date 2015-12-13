#include "traitement_recu.h"

int traiter_recu (char * requete_recu)
{
	unsigned char requete_decrypt[sizeof(requete_recu)];

	decrypt(requete_recu, requete_decrypt);

	char *type = malloc (sizeof (char*) * 256);
	char *donnee = malloc (sizeof (char*) * 256);
	char * save_ptr;

	type = strtok_r(requete_decrypt, "*", &save_ptr);

	int j = 0;

	while (*save_ptr != '\0')	
	{
		donnee[j] = *(save_ptr++);
		j++;
	}

	if (type[0] == 'R') printf("DONNEE de la reponse -> %s\n", donnee);

	return 0;

}