#include "recup_config.h"

/*======================================================================
**  Nom          : recup_valeur_entier
**  Description  : Permet de recupérer un paramètre dans le fichier
**                 conf.ini, et retourne le parametre en entier
** ---------------------------------------------------------------------
**  Parametres   :
**      char *param : parametre recherché dans le fichier de configuration
**======================================================================*/

int recup_valeur_entier ( char* param )
{
    return atoi ( recup_valeur ( param ) ) ;
}

/*======================================================================
**  Nom          : recup_valeur
**  Description  : Permet de recupérer un paramètre dans le fichier
**                 conf.ini, et retourne le parametre en chaine de 
**                 caractere
** ---------------------------------------------------------------------
**  Parametres   :
**      char *param : parametre recherché dans le fichier de configuration
**======================================================================*/

char* recup_valeur ( char* param )
{
    // Déclaration variables
    FILE *fichier ;
    char *ligne = NULL ;
    size_t longueur ;
    ssize_t lecture ;
    int i = 0, j = 0 ;
    int temp = 0 ;

    // On ouvre le fichier
    fichier = fopen ( "config.ini", "r" ) ;
    if  ( fichier == NULL )
    {
        exit ( 1 ) ;
    }

    // On lit ligne par ligne
    while ( ( lecture = getline ( &ligne, &longueur, fichier ) ) != -1 )
    {
        j = 0 ;
        temp = 1 ;
        for ( i = 0; ligne[i] != '\n' && ligne[i] != EOF && temp == 1; i++ )
        {
            switch ( ligne[i] )
            {
                case ' ' :
                    continue ;
                    break ;

                case ';' :
                    temp = 1 ;
                    break ;

                case '=' :
                    temp = 2 ;
                    i++ ;
                    continue ;
            }

            if ( temp == 1 && ligne[i] == param[j] )
                j++ ;
            else
                temp = 0 ;
        }

        if ( temp == 2 )
        {
            int truc = strlen ( ligne ) - i ;
            char *res = malloc ( truc * sizeof ( char ) ) ;
            strcpy ( res, &ligne[i] ) ;

            // On ferme le fichier
            fclose ( fichier ) ;
            res [strlen ( ligne ) - i] = '\0' ;
            i = 0 ;
            while ( res[i] != '\0' )
            {
                if ( res[i] == '\n' )
                    res[i] = '\0' ;
                i++ ;
            }
            if  ( ligne )
                free ( ligne ) ;
            return res ;
        }
    }

    // Retour de la focntion
    return NULL ;
}