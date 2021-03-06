﻿/** ====================================================================
**   Auteur  : Delvarre / Bourillon      | Date    : 07/01/2016
**  --------------------------------------------------------------------
**   Langage : C                         | Systeme : Linux
**  --------------------------------------------------------------------
**   Nom fichier : main.c                | Version : 1.0
**  --------------------------------------------------------------------
**   Description : Programme principal
** =====================================================================*/
#include "default.h"
#include "bdd.h"
#include "crypt.h"
#include "res.h"


// Main
int main ( int argc, char **argv )
{
    // Déclarations variables
    int return_value ;              // Entier recevant les codes de retours des fonctions

    // Initialisation de rand()
    srand ( time ( NULL ) ) ;

    // Initialisation librairie OpenSSL
    OpenSSL_add_all_algorithms () ;
    OPENSSL_config ( NULL ) ;

    // Connexion à la BDD
    mysql_bdd = bdd_start_connection () ;   // On se connecte à la BDD

    // On vérifie la connexion
    if ( mysql_bdd != NULL )
    {
        // On initialise la clé AES
        if ( AES_generate_key() != ERRNO )
        {
            // On initialise la double clé RSA
            if ( generate_RSA_keys() != ERRNO )
            {
                // On vide la BDD par protection
                if ( ( return_value = bdd_do_request ( mysql_bdd, DELETE_ALL, NULL, NULL, NULL ) ) == TRUE )
                {
                    // On initialise le réseau
                    if ( ( return_value = res_activation() ) != ERRNO )
                    {
                        // Un client s'est bel et bien connecté, on lance le thread qui recevra les requêtes
                        if ( ( return_value = pthread_create ( &thread_receive, NULL, fct_thread_res_receive, NULL ) ) == 0 )
                        {
                            // On met en place le egstionnaire de signal sur SIGINT (Ctrl + C)
                            signal ( SIGINT, gestionnaire_signal ) ;

                            // On se place en attente
                            while ( !fermeture ) ;

                            // On kill tous les thread traitement
                            int i ;
                            for ( i = 0; i < NB_MAX_REQ; i++ )
                            {
                                if ( thread_traitement[i] != ZERO )
                                    pthread_cancel ( thread_traitement[i] ) ;
                            }

                            // On kill le thread
                            pthread_cancel ( thread_receive ) ;
                        }
                    }

                    // Finalisation
                    return_value = bdd_do_request ( mysql_bdd, DELETE_ALL, NULL, NULL, NULL ) ;
                    bdd_close_connection ( mysql_bdd ) ;    // On se déconnecte de la BDD
                    res_close() ;                           // On se déconnecte du réseau
                    RSA_free ( bdd_key ) ;                  // On free nos clés RSA & AES
                    free ( exg_key ) ;
                    EVP_cleanup () ;                        // Finalisation OpenSSL
                    printf ( "\nFermeture done \n" ) ;
                    return EXIT_SUCCESS ;
                }
                else
                {
                    EVP_cleanup () ;    // Finalisation OpenSSL
                    return EXIT_FAILURE ;
                }
            }
            else
            {
                EVP_cleanup () ;    // Finalisation OpenSSL
                return EXIT_FAILURE ;
            }
        }
        else
        {
            EVP_cleanup () ;    // Finalisation OpenSSL
            return EXIT_FAILURE ;
        }
    }
    else
    {
        EVP_cleanup () ;    // Finalisation OpenSSL
        return EXIT_FAILURE ;
    }
}
