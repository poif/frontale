/** ====================================================================
**   Auteur  : Delvarre / Bourillon      | Date    : 07/01/2016
**  --------------------------------------------------------------------
**   Langage : C                         | Systeme : Linux
**  --------------------------------------------------------------------
**   Nom fichier : res.c                 | Version : 1.0
**  --------------------------------------------------------------------
**   Description : Gestion du réseau
** =====================================================================*/
#include "res.h"
#include "bdd.h"


//----------------------------------------------------------
// Initialisation des variables globales
//----------------------------------------------------------
int fermeture = FALSE ;


//----------------------------------------------------------
// void gestionnaire_signal ()
//----------------------------------------------------------
// Permet de fermer proprement le programme en attendant le signal SIGINT (Ctrl + C )

void gestionnaire_signal ()
{
    // On remet en place le gestionnaire
    signal ( SIGINT, gestionnaire_signal ) ;
    printf ( "\nFermeture du programme BDD \n" ) ;
    fermeture = TRUE ;
}


//----------------------------------------------------------
// int res_activation ()
//----------------------------------------------------------
// Permet d'activer les paramètres réseaux de la BDD et d'attendre la connexion d'un client (une frontale)

int res_activation ()
{
    // Déclaration variables
    int return_value ;
    struct timeval attente ;
    fd_set rfds ;
    socklen_t taille ;

    // Initialisation errno
    errno = 0 ;

    // Création de la socket (récupération du descripteur)
    socket_server = socket ( AF_INET, SOCK_STREAM, 0 ) ;
    if ( errno != 0 )   // Traitement errno
        return ERRNO ;

    // Initialisation errno
    errno = 0 ;

    // On désactive le "Address already in use"
    int yes = 1 ;
    return_value = setsockopt ( socket_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof ( int ) ) ;
    if ( errno != 0 )
        return ERRNO ;

    // Initialisation errno
    errno = 0 ;

    // Affectation d'une adresse
    addr_server.sin_family = AF_INET ;
    addr_server.sin_addr.s_addr = htonl ( INADDR_ANY ) ;
    addr_server.sin_port = htons ( PORT ) ;
    return_value = bind ( socket_server, (struct sockaddr*) &addr_server, sizeof ( addr_server ) ) ;
    if ( errno != 0 )   // Traitement errno
        return ERRNO ;

    // Initialisation errno
    errno = 0 ;

    // Mise en mode passive de la socket (mode connecté)
    return_value = listen ( socket_server, 1 ) ;        // On limite à une connexion simultannée (1 seule Frontale de co normalement)
    if ( errno != 0 )   // Traitement errno
        return ERRNO ;

    // Initialisation errno
    errno = 0 ;

    // On met en place le egstionnaire de signal sur SIGINT (Ctrl + C)
    signal ( SIGINT, gestionnaire_signal ) ;

    // On initialise l'attente
    FD_ZERO ( &rfds ) ;
    FD_SET ( socket_server, &rfds ) ;
    attente.tv_sec = 5 ;
    attente.tv_usec = 0 ;

    // On se place en attente
    printf ( "\nPour fermer le programme : Ctrl + C\n" ) ;
    while ( !fermeture )
    {
        return_value = select ( socket_server + 1, &rfds, NULL, NULL, &attente ) ;
        if ( return_value  == -1 )
            return ERRNO ;
        else if ( return_value == 1 )   // Client présent
        {
            // Initialisation errno
            errno = 0 ;

            // Un client se connecte (attente bloquante)
            taille = sizeof ( addr_client ) ;
            socket_client = accept ( socket_server, (struct sockaddr*) &addr_client, &taille ) ;
            if ( errno != 0 )   // Traitement errno
                return ERRNO ;
            else
                fermeture = QUATRE ;
        }
        else
        {
            // On remet une attente de 5 sec
            FD_ZERO ( &rfds ) ;
            FD_SET ( socket_server, &rfds ) ;
            attente.tv_sec = 5 ;
            attente.tv_usec = 0 ;
        }
    }

    // Traitement de fermeture
    if ( fermeture == QUATRE )
        fermeture = FALSE ;
    else
        return ERRNO ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// void res_close ()
//----------------------------------------------------------
// Permet de fermer toutes les sockets réseaux ouvertes

void res_close ()
{
    // Fermeture des sockets
    close ( socket_server ) ;
    close ( socket_client ) ;
}


//----------------------------------------------------------
// void res_receive ()
//----------------------------------------------------------
// Permet d'écouter le réseau afin de récupérer les requêtes envoyées par la frontale

void res_receive ()
{
    // Initialisation
    char trame[TAILLE_MAX_TRAME] ;
    int return_value ;
    fd_set rfds ;
    int nc ;
    int i, j ;
    int stop ;

    // Initialisation thread_req
    for ( i = 0; i < NB_MAX_REQ; i++ )
        thread_traitement[i] = ZERO ;

    // Initialisation tab_req
    char tab_req[NB_MAX_REQ][TAILLE_MAX_TRAME] ;
    for ( i = 0; i < NB_MAX_REQ; i++ )
        memset ( &tab_req[i][0], '\0', TAILLE_MAX_TRAME ) ;

    // Initialisation mutex
    pthread_mutex_init ( &mutex_bdd, NULL);

    // Boucle infinie : attente de requêtes
    while ( 1 )
    {
        // Ini trame
        memset ( &trame[0], '\0', TAILLE_MAX_TRAME ) ;

        // Initialisation errno
        errno = 0 ;

        // On place un select bloquant sur la lecture pour attendre changement d'état
        FD_ZERO ( &rfds ) ;
        FD_SET ( socket_client, &rfds ) ;
        return_value = select ( socket_client + 1, &rfds, NULL, NULL, NULL ) ;
        if ( return_value == 1 )   // Message
        {
            // Réception du message
            nc = recv ( socket_client, trame, TAILLE_MAX_TRAME, 0 ) ;

            // Traitement du type de message
            if ( nc == 0 )     // Déco client
            {
                fermeture = TRUE ;
                while ( 1 ) ;
            }
            else
            {

                // On envoie le traitement dans un thread
                stop = FALSE ;
                for ( i = 0; i < NB_MAX_REQ; i++ )
                {
                    if ( stop == FALSE )
                    {
                        if ( thread_traitement[i] == ZERO )
                        {
                            // On prépare arg
                            memset ( &tab_req[i][0], '\0', TAILLE_MAX_TRAME ) ;
                            memcpy ( &tab_req[i][0], trame, TAILLE_MAX_TRAME ) ;

                            // On thread
                            if ( ( return_value = pthread_create ( &thread_traitement[i], NULL, fct_thread_do_req, (void*) tab_req[i] ) ) == 0 )
                            {
                                stop = TRUE ;   // On stop

                                // On nettoie (boucle récurrente)
                                if ( i == 9 )
                                {
                                    for ( j = 10; j < NB_MAX_REQ; j++ )
                                        thread_traitement[j] = ZERO ;
                                }
                                if ( i == 19 )
                                {
                                    for ( j = 0; j < 10; j++ )
                                        thread_traitement[j] = ZERO ;
                                }
                            }
                            else
                            {
                                stop = ERRNO ;
                                bdd_send_msg ( 0, ERROR, "THREAD", FALSE ) ;
                            }
                        }
                    }
                }

                // Debug
                if ( stop == FALSE )
                {
                    bdd_send_msg ( 0, ERROR, "THREAD", FALSE ) ;
                }
            }
        }
    }
}


//----------------------------------------------------------
// void* fct_thread_res_receive ()
//----------------------------------------------------------
// Fonction pointeur pour thread

void* fct_thread_res_receive ()
{
    res_receive () ;
    return NULL ;
}


//----------------------------------------------------------
// void* fct_thread_do_req ( void* p_data )
//----------------------------------------------------------
// Fonction pointeur pour thread

void* fct_thread_do_req ( void* p_data )
{
    // Ini
    int terminer = FALSE ;

    // On récupère la trame
    char* trame = (char*) p_data ;

    // Attente exécution req
    while ( terminer != TRUE )
    {
        // On mutex
        pthread_mutex_lock ( &mutex_bdd ) ;

        // On transmet la trame pour le traitement
        bdd_parser ( trame ) ;

        // On quitte
        terminer = TRUE ;

        // End mutex
        pthread_mutex_unlock ( &mutex_bdd ) ;
    }

    // Return
    return NULL ;
}

//----------------------------------------------------------
// void res_send ( char *trame, int len )
//----------------------------------------------------------
// Permet d'envoyer des trames sur le réseau vers le client

void res_send ( char *trame, int len )
{
    send ( socket_client, trame, len, 0 ) ;
}
