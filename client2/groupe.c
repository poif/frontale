#include "groupe.h"

char * recup_groupe()
{
	FILE *fichier_groupe ;
	char *group = malloc (sizeof (char*) *2048);
	char *ligne = NULL;
	ssize_t lecture;
	ssize_t longueur;

	fichier_groupe = fopen ("groupe.ini", "r");

	if(fichier_groupe == NULL) puts("Erreur ouverture fichier");

	int group_compt = 0;

	while ( ( lecture = getline (&ligne, &longueur, fichier_groupe) ) != -1 )
	{
		int i;

		for(i = 0; ligne[i] != '\n' && ligne[i] != EOF; i++)
		{
			group[group_compt] = ligne[i];
			group_compt++;
		}
		group[group_compt++] = '*';
	}

	close(fichier_groupe);

	return group;
}

void create_group(char * group_name)
{
	FILE *fichier_groupe;

	fichier_groupe = fopen("groupe.ini", "w");

	fprintf(fichier_groupe, "%s\n", group_name);

	close(fichier_groupe);
}