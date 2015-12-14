#ifndef CRYPT_H
#define CRYPT_H


//----------------------------------------------------------
// Déclarations des librairies extérieures
//----------------------------------------------------------
#include "default.h"
#include "openssl/sha.h"
#include "openssl/rsa.h"
#include <openssl/err.h>


//----------------------------------------------------------
// Déclarations des constantes
//----------------------------------------------------------
#define CHIFFREMENT         600
#define DECHIFFREMENT       601
#define KEY_LENGTH          2048
#define PADDING_FLEN        11


//----------------------------------------------------------
// Déclarations des variables globales
//----------------------------------------------------------
extern RSA* bdd_key ;


//----------------------------------------------------------
// Prototypes des fonctions
//----------------------------------------------------------
unsigned long calculate_SHA256 ( char* input ) ;
unsigned long multiple ( unsigned long input ) ;
int generate_RSA_keys () ;
int RSA_chiffrement ( unsigned char* input, unsigned char* output, int mode ) ;


#endif // CRYPT_H

