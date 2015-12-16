/** ====================================================================
**   Auteur  : Delvarre / Bourillon      | Date    : DD/MM/2015
**  --------------------------------------------------------------------
**   Langage : C                         | Systeme : Linux
**  --------------------------------------------------------------------
**   Nom fichier : crypt.c               | Version : 1.0
**  --------------------------------------------------------------------
**   Description : /
** =====================================================================*/
#include "crypt.h"


//----------------------------------------------------------
// Initialisation des variables globales
//----------------------------------------------------------
RSA* bdd_key = NULL ;


//----------------------------------------------------------
// unsigned long calculate_SHA256 ( char* input )
//----------------------------------------------------------
// Permet de calculer le hash SHA256 d'une chaîne de caractère et de le retourner sous forme d'un entier (4 octets max)

unsigned long calculate_SHA256 ( char* input )
{
    // Vérification
    if ( strlen ( input ) == FALSE )
    {
        perror ( "Erreur_calculate_SHA256 : chaine nulle " ) ;
        return ERRNO ;
    }

    // Initialisation
    unsigned char hash[SHA256_DIGEST_LENGTH] ;  // Résultat du hash
    SHA256_CTX sha256 ;                         // Structure utilisée
    unsigned long res  = 0x0 ;                  // Résultat
    int mul = 1 ;
    int i ;

    // On initialise la structure
    if ( SHA256_Init ( &sha256 ) == 0 )
    {
        perror ( "Erreur_calculate_SHA256 : SHA256_Init " ) ;
        return ERRNO ;
    }

    // On update avec la châine à calculer
    if ( SHA256_Update ( &sha256, input, strlen ( input ) ) == 0 )
    {
        perror ( "Erreur_calculate_SHA256 : SHA256_Update " ) ;
        return ERRNO ;
    }


    // On calcul le hash
    if ( SHA256_Final ( hash, &sha256 ) == 0 )
    {
        perror ( "Erreur_calculate_SHA256 : SHA256_Final " ) ;
        return ERRNO ;
    }

    // On le transforme en entier (on prend les 6 dernières lettres)
    for ( i = 0; i < 3; i++ )
    {
        res = res +  mul * hash[SHA256_DIGEST_LENGTH - 1 - i ] ;
        mul = mul * 16 * 16 ;
    }

    // On retourne le résultat
    return res ;
}


//----------------------------------------------------------
// unsigned long multiple ( unsigned long input )
//----------------------------------------------------------
// Permet de retourner un multiple de l'entrée (de *2 à *256)

unsigned long multiple ( unsigned long input )
{
    unsigned long multiple = rand() % ( 257 - 2 ) + 2 ;
    return input * multiple ;
}


//----------------------------------------------------------
// int generate_RSA_keys ()
//----------------------------------------------------------
// Permet de générer une paire de clés RSA pour le programme BDD

int generate_RSA_keys ()
{
    // DEBUG
    printf ( "Generation cles RSA ... \n" ) ;
    // Déclaration variables
    BIGNUM *bne = NULL ;        // Exponent
    int bits = KEY_LENGTH ;     // Taille clé

    // On alloue notre object RSA
    if ( ( bdd_key = RSA_new() ) == NULL )
    {
        perror ( "Erreur_generate_RSA_keys : RSA_New() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return ERRNO ;
    }

    // On alloue notre object BIGNUM
    if ( ( bne = BN_new() ) == NULL )
    {
        perror ( "Erreur_generate_RSA_keys : BN_new() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return ERRNO ;
    }

    // On fixe l'exposant sur la valeur maximale courante
    if ( ( BN_set_word ( bne, 65537 ) ) == 0 )
    {
        perror ( "Erreur_generate_RSA_keys : BN_set_word() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return ERRNO ;
    }

    // On génère maintenant notre paire de clés RSA
    if ( ( RSA_generate_key_ex ( bdd_key, bits, bne, NULL ) ) != 1 )
    {
        perror ( "Erreur_generate_RSA_keys : RSA_generate_key_ex() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return ERRNO ;
    }

    // On free notre object BIGNUM
    BN_free ( bne ) ;

    // Dernière vérification
    if ( bdd_key == NULL )
    {
        perror ( "Erreur_generate_RSA_keys : impossible de generer les cles " ) ;
        return ERRNO ;
    }
    else
        return TRUE ;
}


//----------------------------------------------------------
// int RSA_chiffrement ( unsigned char* input, unsigned char* output, int mode )
//----------------------------------------------------------
// Permet de chiffrer avec notre clé publique et déchiffrer avec notre clé privée

int RSA_chiffrement ( unsigned char* input, unsigned char* output, int mode )
{
    // initialisation
    int flen = RSA_size ( bdd_key ) - PADDING_FLEN ;  // because RSA_PKCS1_PADDING : see doc

    // On commence par vérifier l'intégrité de notre paire de clés
    if ( RSA_check_key ( bdd_key ) != 1 )
    {
        perror ( "Erreur_RSA_chiffrement : invalid RSA keys " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return ERRNO ;
    }

    if ( mode == CHIFFREMENT )
    {
        // On chiffre, toujours avec la clé publique
        if ( ( RSA_public_encrypt ( flen , input, output, bdd_key, RSA_PKCS1_PADDING ) ) <= 0 )
        {
            perror ( "Erreur_RSA_chiffrement : error chiffrement " ) ;
            fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
            return ERRNO ;
        }
    }
    else if ( mode == DECHIFFREMENT )
    {
        // On déchiffre, toujours avec la clé privée
        if ( ( RSA_private_decrypt ( flen + PADDING_FLEN, input, output, bdd_key, RSA_PKCS1_PADDING) ) <= 0 )
        {
            perror ( "Erreur_RSA_dechiffrement : error déchiffrement " ) ;
            fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
            return ERRNO ;
        }
    }
    else
    {
        perror ( "Erreur_RSA_chiffrement : bad mode " ) ;
        return ERRNO ;
    }

    // On indique que tout s'est bien déroulé
    return TRUE ;
}
