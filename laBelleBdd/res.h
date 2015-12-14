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
#define PORT                2211
#define TAILLE_MAX_TRAME    245


//----------------------------------------------------------
// Déclarations des variables globales
//----------------------------------------------------------
extern int fermeture ;
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
void res_send ( char *trame ) ;


#endif // RES_H

