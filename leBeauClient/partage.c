#include "partage.h"

void partage_create(char *nom, char *path, char *type, int bdd, char *group_name)
{
	FILE *fichier_partage;

	fichier_partage = fopen ("partage.ini", "w");

	fprintf(fichier_partage, "%s;%s;%s;%i;%s\n", nom, path, type, bdd, group_name);

	fclose (fichier_partage);
}

int partage_demande(char *compare, int recherche)
{

	 // Déclaration variables
    FILE *fichier ;
    char *ligne = NULL ;
    size_t longueur ;
    ssize_t lecture ;
    char * save_ptr;
    char *nom = malloc (sizeof (char*) * 1024);
    char *path = malloc (sizeof (char*) * 1024);
    char *type = malloc (sizeof (char*) * 1024);
    char *type_partage = malloc (sizeof (char*) * 2);
    char *groupe = malloc (sizeof (char*) * 2);

    // On ouvre le fichier
    fichier = fopen ( "partage.ini", "r" ) ;
    if  ( fichier == NULL )
    {
        exit ( 1 ) ;
    }

    // On lit ligne par ligne
    while ( ( lecture = getline ( &ligne, &longueur, fichier ) ) != -1 )
    {
    	nom = strtok_r(ligne, ";", &save_ptr);
    	path = strtok_r(NULL, ";", &save_ptr);
    	type = strtok_r(NULL, ";", &save_ptr);
    	type_partage = strtok_r(NULL, ";", &save_ptr);
    	groupe = strtok_r(NULL, ";", &save_ptr);
      printf("incoming\n");
      printf("type : %c\n", type[0]);

    	switch(recherche)
		{
			case S_TYPE:
			{
				if (strncmp (compare, type, strlen(compare)) == 0) 
					{ fclose(fichier); return 1; }
				break;
			}

			case S_GROUPE:
			{
				if (strcmp (compare, groupe) == 0) 
					{ fclose(fichier); return 1; }
				break;
			}
		}
        
    }
    fclose(fichier);
    // Retour de la focntion
    return 0 ;
}

char *retour_path(char *reference)
{
		 // Déclaration variables
    FILE *fichier ;
    char *ligne = NULL ;
    size_t longueur ;
    ssize_t lecture ;
    char * save_ptr;
    char *nom = malloc (sizeof (char*) * 1024);
    char *path = malloc (sizeof (char*) * 1024);

    // On ouvre le fichier
    fichier = fopen ( "partage.ini", "r" ) ;
    if  ( fichier == NULL )
    {
        exit ( 1 ) ;
    }

    // On lit ligne par ligne
    while ( ( lecture = getline ( &ligne, &longueur, fichier ) ) != -1 )
    {
    	nom = strtok_r(ligne, ";", &save_ptr);
    	path = strtok_r(NULL, ";", &save_ptr);
    }

    fclose(fichier);

    return path;

}

char *retour_reference(char * extension)
{

	// Déclaration variables
    FILE *fichier ;
    char *ligne = NULL ;
    size_t longueur ;
    ssize_t lecture ;
    char * save_ptr;
    char *nom = malloc (sizeof (char*) * 1024);
    char *path = malloc (sizeof (char*) * 1024);
    char *extension_file = malloc (sizeof (char*) * 1024);

    // On ouvre le fichier
    fichier = fopen ( "partage.ini", "r" ) ;
    if  ( fichier == NULL )
    {
        exit ( 1 ) ;
    }

    // On lit ligne par ligne
    while ( ( lecture = getline ( &ligne, &longueur, fichier ) ) != -1 )
    {
    	nom = strtok_r(ligne, ";", &save_ptr);
    	path = strtok_r(NULL, ";", &save_ptr);
    	extension_file = strtok_r(NULL, ";", &save_ptr);
    	if(strcmp(extension, extension_file) == 0) 
    	{
    		    fclose(fichier);
    			return nom;
    	}
    }
    fclose(fichier);
}

void partage_delete(char *nom/*, char *path*/)
{
	char fichier[] = "partage.ini";
    struct stat st;
    if ( stat( fichier, &st ) != -1 )
    {
      FILE* fichier_partage = fopen( fichier, "rb" );

      if ( fichier_partage != NULL )
      {
        char* buffer = malloc( st.st_size ); 

        if ( fread(buffer, 1, st.st_size, fichier_partage) == st.st_size)
        {
          fclose(fichier_partage);

          size_t newSize = suppression_ligne( buffer, st.st_size, nom );

          fichier_partage = fopen( fichier, "wb" );
          if ( fichier_partage != NULL )
          {
            fwrite(buffer, 1, newSize, fichier_partage);
            fclose(fichier_partage);
          }
          else
          {
            perror(fichier);
          }
        }
      }
      else
      {
        perror(fichier);
      }
    }
    else
    {
      puts("Erreur fichier partage");
    }
 }

static size_t suppression_ligne( char* buffer, size_t size, const char* recherche )
{

  char* buf = buffer; 
  bool fin = false;
  size_t longueur = strlen(recherche);
  size_t NouvelleTaille = 0;
  do
  {
    char* first = strchr( buf, *recherche );
    if ( first != NULL )
    {
      if ( strncmp( first, recherche, longueur ) == 0 ) 
      {
        size_t LongueurLigne = 1; 

        
        for ( char* ligne = first; *ligne != '\n'; ++ligne) 
        {
          ++LongueurLigne;
        }

        size_t reste = (size_t)((buffer + size) - (first + LongueurLigne));

        // changement block avec la ligne suivante
        memmove( first, first + LongueurLigne, reste );

        NouvelleTaille = size - LongueurLigne;
        fin = true;
      }
      else
      {
        buf = first + 1; //pôursuite recherche
      }
    }
    else
    {
      puts( "no such name" );
      fin = true;
    }
  } while (!fin);

  return NouvelleTaille;
}