#ifndef RES_H
#define RES_H


//----------------------------------------------------------
// Déclarations des librairies extérieures
//----------------------------------------------------------
#include "default.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


//----------------------------------------------------------
// Déclarations des constantes
//----------------------------------------------------------
#define PORT                22666
#define TAILLE_MAX_TRAME    450
#define NB_MAX_REQ          25


//----------------------------------------------------------
// Déclarations des variables globales
//----------------------------------------------------------
extern int fermeture ;
pthread_t thread_traitement[NB_MAX_REQ] ;
pthread_mutex_t mutex_bdd ;
pthread_t thread_receive ;
struct sockaddr_in  addr_server ;
struct sockaddr_in  addr_client ;
int socket_server ;
int socket_client ;


//----------------------------------------------------------
// Prototypes des fonctions
//----------------------------------------------------------
void gestionnaire_signal () ;
int res_activation () ;
void res_close () ;
void res_receive () ;
void* fct_thread_res_receive () ;
void* fct_thread_do_req ( void* p_data ) ;
void res_send ( char *trame, int len ) ;


#endif // RES_H

