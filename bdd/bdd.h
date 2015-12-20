#ifndef BDD_H
#define BDD_H


//----------------------------------------------------------
// Déclarations des librairies extérieures
//----------------------------------------------------------
#include "default.h"
#include <mysql/mysql.h>


//----------------------------------------------------------
// Déclarations des constantes
//----------------------------------------------------------
#define HOST                    "localhost"
#define USER                    "USER_ONE"
#define PWD                     "AZERTY"
#define DB                      "ONE"
#define TABLE                   "STOCKAGE"

#define SEEK                    300
#define SELECT                  301
#define INSERT                  302
#define DELETE                  303
#define INIT                    304
#define ERROR                   305
#define SELECT_ALL              200
#define DELETE_ALL              201

#define STATUT                  0
#define AFFECTATION             1
#define GROUPE                  2
#define TYPE                    3

#define TAILLE_MAX_REF          33
#define TAILLE_MAX_SUB_REF      9
#define TAILLE_HEXA             16
#define TAILLE_HASH_NOM         21
#define TAILLE_MAX_RSA          256
#define TAILLE_MAX_UTILE        TAILLE_MAX_RSA - 11
#define TAILLE_REQUETE          50 + strlen ( TABLE ) + TAILLE_MAX_REF + TAILLE_MAX_RSA

#define APOSTROPHE              34
#define ZERO_TERMINAL           0
#define BACK_SLASH              92


//----------------------------------------------------------
// Définition de la structure Ref
//----------------------------------------------------------
typedef struct Ref
{
    unsigned long statut ;
    unsigned long affectation ;
    unsigned long groupe ;
    unsigned long type ;
} Ref ;


//----------------------------------------------------------
// Déclarations des variables globales
//----------------------------------------------------------
extern MYSQL* mysql_bdd ;
extern int nbr_tuples ;


//----------------------------------------------------------
// Prototypes des fonctions bdd.c
//----------------------------------------------------------
MYSQL* bdd_start_connection () ;
void bdd_close_connection ( MYSQL* mysql_bdd ) ;
int ref_concatenation ( Ref* reference, char* reference_finale ) ;
int ref_split ( char* reference_input, Ref* reference ) ;
int ref_cmp ( Ref* reference, Ref* modele ) ;
int bdd_verification_partage ( char* politiques, char* atester ) ;
void bdd_send_msg ( int flag, char* texte ) ;


//----------------------------------------------------------
// Prototypes des fonctions bdd_request.c
//----------------------------------------------------------
int bdd_parser ( char *trame ) ;
int bdd_do_insert ( char *safe_trame ) ;
int bdd_do_delete ( char *safe_trame ) ;
int bdd_do_seek ( char *safe_trame ) ;
int bdd_do_select ( char *safe_trame ) ;
int bdd_search_nom ( char* reference, char* nom ) ;
int bdd_search_ref ( Ref* modele ) ;
int bdd_search_modele ( MYSQL_RES* resultat_req, Ref* modele ) ;
int bdd_do_request ( MYSQL* mysql_bdd, int action, char* reference, char* valeur, MYSQL_RES** resultat_req ) ;


#endif // BDD_H

