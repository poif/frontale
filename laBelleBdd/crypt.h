#ifndef CRYPT_H
#define CRYPT_H


//----------------------------------------------------------
// Déclarations des librairies extérieures
//----------------------------------------------------------
#include "default.h"
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>


//----------------------------------------------------------
// Déclarations des constantes
//----------------------------------------------------------
#define CHIFFREMENT         1
#define DECHIFFREMENT       0
#define KEY_LENGTH          2048
#define PADDING_FLEN        11
#define AES_CKEY_LENGTH     33
#define AES_IVEC_LENGTH     17


//----------------------------------------------------------
// Définition de la structure AES_duo
//----------------------------------------------------------
typedef struct AES_duo
{
    unsigned char ckey[AES_CKEY_LENGTH] ;
    unsigned char ivec[AES_IVEC_LENGTH] ;
} AES_duo ;


//----------------------------------------------------------
// Déclarations des variables globales
//----------------------------------------------------------
extern AES_duo* exg_key ;
extern RSA* bdd_key ;
extern RSA* frt_key ;


//----------------------------------------------------------
// Prototypes des fonctions
//----------------------------------------------------------
unsigned long calculate_SHA256 ( char* input ) ;
unsigned long multiple ( unsigned long input ) ;
int generate_RSA_keys () ;
int RSA_chiffrement ( unsigned char* input, unsigned char* output, int mode ) ;
int RSA_chiffrement_aes ( unsigned char* input, unsigned char* output ) ;
int RSA_recup_key_pub ( unsigned char* key ) ;
int AES_generate_key () ;
int AES_chiffrement ( char* input, char* output, int len, int* ilen, int mode ) ;
int AES_envoi_key_aes ( int id ) ;


#endif // CRYPT_H

