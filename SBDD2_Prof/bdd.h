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
#define USER                    "njibhuvgycft"
#define PWD                     "Qu4NdJ353r415Gr4ND"
#define DB                      "qsdfghjklm"
#define TABLE                   "azertyuiop"

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
#define TAILLE_HASH_NOM         41
#define TAILLE_MAX_RSA          256
#define TAILLE_MAX_UTILE        TAILLE_MAX_RSA - 11
#define TAILLE_REQUETE          50 + strlen ( TABLE ) + TAILLE_MAX_REF + TAILLE_MAX_RSA

#define APOSTROPHE              34
#define ZERO_TERMINAL           0
#define BACK_SLASH              92
#define ETOILE                  42


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
extern int traitement ;


//----------------------------------------------------------
// Prototypes des fonctions bdd.c
//----------------------------------------------------------
MYSQL* bdd_start_connection () ;
void bdd_close_connection ( MYSQL* mysql_bdd ) ;
int ref_concatenation ( Ref* reference, char* reference_finale ) ;
int ref_split ( char* reference_input, Ref* reference ) ;
int ref_cmp ( Ref* reference, Ref* modele ) ;
int bdd_verification_partage ( char* politiques, char* atester ) ;
void bdd_send_msg ( int id, int flag, char* texte, int chiffre ) ;
int bdd_send_reponse ( char *texte, int id, int code ) ;


//----------------------------------------------------------
// Prototypes des fonctions bdd_request.c
//----------------------------------------------------------
int bdd_parser ( char *trame ) ;
int bdd_do_init ( int id, char *safe_trame ) ;
int bdd_do_insert ( int id, char *safe_trame ) ;
int bdd_do_delete ( int id, char *safe_trame ) ;
int bdd_do_seek ( int id, char *safe_trame ) ;
int bdd_do_select ( int id, char *safe_trame ) ;
int bdd_search_nom ( char* reference, char* nom ) ;
int bdd_search_ref ( int id, Ref* modele ) ;
int bdd_search_modele ( int id, MYSQL_RES* resultat_req, Ref* modele ) ;
int bdd_do_request ( MYSQL* mysql_bdd, int action, char* reference, char* valeur, MYSQL_RES** resultat_req ) ;


#endif // BDD_H

