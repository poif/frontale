#include "partage.h"

void partage_create(char *nom, char *path, int bdd, char *group_name)
{
	FILE *fichier_partage;

	fichier_partage = fopen ("partage.ini", "w");

	fprintf(fichier_partage, "%s\t%s\t%i\t%s\n", nom, path, bdd, group_name);

	close (fichier_partage);
}

void partage_delete(char *nom, char *path)
{
	FILE *fichier_partage;

	fichier_partage = fopen ("partage.ini", "w");

	

	close (fichier_partage);

}