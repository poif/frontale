#include "client.h"
#include "crypt.h"

int main (void)
{
	char * command = malloc (sizeof (char*) *1024);
	//unsigned char entre[] = "bonjourj'aimelecheval";
	//unsigned char sortie[sizeof(entre)];// = malloc (sizeof(entre));
	//unsigned char lafin[sizeof(entre)];// = malloc (sizeof(entre));

	partage_create("fichier.txt", "/home/valou/fichier.txt", 0, "mongroup,niania");

	if(memcmp(recup_valeur("accueil_visible"), "true", 4) == 0)
		accueil();

	if(ecoute_client() != 0)
	{
		puts("Erreur lors de la creation du thread");
		return -1;
	}

	//crypt(entre, sortie, sizeof(entre));
	//decrypt(sortie, lafin, sizeof(entre));

	//printf("La fin -> %s\n", lafin);

	puts("Entrez une commande ou help !");

	do
	{
		printf(">>> ");
		memset(command, '\0', 1024);
		int i = 0;
		char c;

		while ( (c = getc(stdin)) != '\n') 
			{
				command[i] = c;
				i++;
			}

	}while ( traiter_commande(command) == true );

	ecoute_kill_thread();

	return 0;
}

/*======================================================================
**  Nom          : accueil
**  Description  : Permet l'affichage de l'ascii-art d'accueil
**======================================================================*/

void accueil()
{
    char *filename = "signature.txt";
    FILE *fptr = NULL;
 
    if((fptr = fopen(filename,"r")) == NULL)
    {
        fprintf(stderr,"error opening %s\n",filename);
        return 1;
    }

    char read_string[256];
 
    while(fgets(read_string,sizeof(read_string),fptr) != NULL)
        printf("%s",read_string);
    fclose(fptr);
    printf("\n\n");
}