/** ====================================================================
**   Auteur  : Delvarre / Bourillon      | Date    : 07/01/2016
**  --------------------------------------------------------------------
**   Langage : C                         | Systeme : Linux
**  --------------------------------------------------------------------
**   Nom fichier : crypt.c               | Version : 1.0
**  --------------------------------------------------------------------
**   Description : Gestion de la sécurité
** =====================================================================*/
#include "crypt.h"
#include "bdd.h"
#include "res.h"


//----------------------------------------------------------
// Initialisation des variables globales
//----------------------------------------------------------
RSA* frt_key = NULL ;
RSA* bdd_key = NULL ;
AES_duo* exg_key = NULL ;


//----------------------------------------------------------
// unsigned long calculate_SHA256 ( char* input )
//----------------------------------------------------------
// Permet de calculer le hash SHA256 d'une chaîne de caractère et de le retourner sous forme d'un entier (4 octets max)

unsigned long calculate_SHA256 ( char* input )
{
    // Vérification
    if ( strlen ( input ) == FALSE )
        return ERRNO ;

    // Initialisation
    unsigned char hash[SHA256_DIGEST_LENGTH] ;  // Résultat du hash
    SHA256_CTX sha256 ;                         // Structure utilisée
    unsigned long res  = 0x0 ;                  // Résultat
    int mul = 1 ;
    int i ;

    // On initialise la structure
    if ( SHA256_Init ( &sha256 ) == 0 )
        return ERRNO ;

    // On update avec la châine à calculer
    if ( SHA256_Update ( &sha256, input, strlen ( input ) ) == 0 )
        return ERRNO ;


    // On calcul le hash
    if ( SHA256_Final ( hash, &sha256 ) == 0 )
        return ERRNO ;

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
    // Déclaration variables
    BIGNUM *bne = NULL ;        // Exponent
    int bits = KEY_LENGTH ;     // Taille clé

    // On alloue notre object RSA
    if ( ( bdd_key = RSA_new() ) == NULL )
        return ERRNO ;

    // On alloue notre object BIGNUM
    if ( ( bne = BN_new() ) == NULL )
        return ERRNO ;

    // On fixe l'exposant sur la valeur maximale courante
    if ( ( BN_set_word ( bne, 65537 ) ) == 0 )
        return ERRNO ;

    // On génère maintenant notre paire de clés RSA
    if ( ( RSA_generate_key_ex ( bdd_key, bits, bne, NULL ) ) != 1 )
        return ERRNO ;

    // On free notre object BIGNUM
    BN_free ( bne ) ;

    // Dernière vérification
    if ( bdd_key == NULL )
        return ERRNO ;
    else
        return TRUE ;
}


//----------------------------------------------------------
// int RSA_chiffrement ( unsigned char* input, unsigned char* output, int mode )
//----------------------------------------------------------
// Permet de chiffrer avec notre clé publique et déchiffrer avec notre clé privée

int RSA_chiffrement ( unsigned char* input, unsigned char* output, int mode )
{
    // Initialisation
    int flen = RSA_size ( bdd_key ) - PADDING_FLEN ;  // because RSA_PKCS1_PADDING : see doc

    // On commence par vérifier l'intégrité de notre paire de clés
    if ( RSA_check_key ( bdd_key ) != 1 )
        return ERRNO ;

    if ( mode == CHIFFREMENT )
    {
        if ( ( RSA_public_encrypt ( flen , input, output, bdd_key, RSA_PKCS1_PADDING ) ) <= 0 )
            return ERRNO ;
    }
    else if ( mode == DECHIFFREMENT )
    {
        if ( ( RSA_private_decrypt ( flen + PADDING_FLEN, input, output, bdd_key, RSA_PKCS1_PADDING) ) <= 0 )
            return ERRNO ;
    }
    else
        return ERRNO ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int RSA_chiffrement_aes ( unsigned char* input, unsigned char* output )
//----------------------------------------------------------
// Permet de chiffrer la clé AES avec la clé publique de la frontale

int RSA_chiffrement_aes ( unsigned char* input, unsigned char* output )
{
    // Initialisation
    int flen = RSA_size ( frt_key ) - PADDING_FLEN ;  // because RSA_PKCS1_PADDING : see doc

    // On chiffre, toujours avec la clé publique de la frontale
    if ( ( RSA_public_encrypt ( flen , input, output, frt_key, RSA_PKCS1_PADDING ) ) <= 0 )
        return ERRNO ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int RSA_recup_key_pub ( unsigned char* key )
//----------------------------------------------------------
// Permet de récupérer la clé RSA publique de la frontale

int RSA_recup_key_pub ( unsigned char* key )
{
    // Déclaration variables
    BIO *keybio = NULL ;

    // On aloue notre object BIO avec la clé
    if ( ( keybio = BIO_new_mem_buf ( key, -1 ) ) == NULL )
        return ERRNO ;

    // On lie la clé à la structure RSA
    if ( ( frt_key = PEM_read_bio_RSAPublicKey ( keybio, &frt_key, NULL, NULL ) ) == NULL )
        return ERRNO ;

    // On free notre object BIO
    BIO_free ( keybio ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int AES_generate_key ()
//----------------------------------------------------------
// Permet de générer 2 ensembles de clés AES, une pour bdd et une pour les échanges avec frontale

int AES_generate_key ()
{
    // Déclaration variables
    int i ;

    // On alloue notre structure
    exg_key = (AES_duo*) malloc ( sizeof ( AES_duo ) ) ;

    // On initialise notre structure
    memset ( exg_key->ckey, '\0', AES_CKEY_LENGTH ) ;
    memset ( exg_key->ivec, '\0', AES_IVEC_LENGTH ) ;

    // On génère les clés et IV avec des caractères aléatoires
    for ( i = 0; i < AES_CKEY_LENGTH - 1; i++ )
    {
        exg_key->ckey[i] = rand() % ( 122 - 97 ) + 97 ;
        if ( i <= AES_IVEC_LENGTH - 1 )
            exg_key->ivec[i] = rand() % ( 122 - 97 ) + 97 ;
    }

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int AES_chiffrement ( char* input, char* output, int len, int* ilen, int mode )
//----------------------------------------------------------
// Permet de chiffrer / déchiffrer en utilisant une clé / iv de type AES_256

int AES_chiffrement ( char* input, char* output, int len, int* ilen, int mode )
{
    // Déclaration variables
    EVP_CIPHER_CTX ctx ;
    int olen1 = 0, olen2 = 0 ;

    // Initialisation de la sortie
    char* out = (char*) malloc ( len * sizeof ( char ) ) ;
    memset ( out, '\0', len ) ;

    // Initialisation du contexte
    EVP_CIPHER_CTX_init ( &ctx ) ;

    // Paramétrage du contexte
    if ( EVP_CipherInit_ex ( &ctx, EVP_aes_256_cbc(), NULL, exg_key->ckey, exg_key->ivec, mode ) == 0 )
    {
        free ( out ) ;
        EVP_CIPHER_CTX_cleanup ( &ctx ) ;
        return ERRNO ;
    }

    // Paramétrage du padding
    if ( mode == DECHIFFREMENT )
        EVP_CIPHER_CTX_set_padding ( &ctx, 0 ) ;

    // On chiffre / déchiffre en utilisant la clé passée en paramètre
    if ( EVP_CipherUpdate ( &ctx, (unsigned char*) out, &olen1, (unsigned char*) input, *ilen ) == 0 )
    {
        free ( out ) ;
        EVP_CIPHER_CTX_cleanup ( &ctx ) ;
        return ERRNO ;
    }

    // On finalise
    if ( EVP_CipherFinal_ex ( &ctx, (unsigned char*) out + olen1, &olen2) == 0 )
    {
        free ( out ) ;
        EVP_CIPHER_CTX_cleanup ( &ctx ) ;
        return ERRNO ;
    }

    // On écrit sur la sortie
    memcpy ( &output[0], out, olen1 + olen2 ) ;
    *ilen = olen1 + olen2 ;

    // On nettoie
    free ( out ) ;
    if ( EVP_CIPHER_CTX_cleanup ( &ctx ) == 0 )
        return ERRNO ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int AES_envoi_key_aes ( int id )
//----------------------------------------------------------
// Permet d'envoyer une clé AES à la frontale

int AES_envoi_key_aes ( int id )
{
    // Initialisation
    char* chiffre = (char*) malloc ( RSA_size ( frt_key ) * sizeof ( char ) ) ;
    memset ( chiffre, '\0', RSA_size ( frt_key ) ) ;

    // On prépare la clé AES
    int taille = AES_CKEY_LENGTH + AES_IVEC_LENGTH + 2 ;
    char* cle_aes = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( cle_aes, '\0', taille) ;
    sprintf ( cle_aes, "%s;%s", exg_key->ckey, exg_key->ivec ) ;

    // On chiffre avec la clé publique de la frontale
    int good = FALSE ;
    int i ;

    // Tant que le chiffré n'est pas bon
    while ( good != TRUE )
    {
        // Ini
        memset ( chiffre, '\0', RSA_size ( frt_key ) ) ;

        // On chiffre
        if ( RSA_chiffrement_aes ( (unsigned char*) cle_aes, (unsigned char*) chiffre ) != TRUE )
        {
            free ( chiffre ) ;
            free ( cle_aes ) ;
            return ERRNO ;
        }

        // On suppose chiffré bon
        good = TRUE ;

        // On vérifie la non présence du caractère ", \ et \0
        for ( i = 0; i < RSA_size ( frt_key ); i ++ )
        {
            if ( ( (int) chiffre[i] == APOSTROPHE ) || ( (int) chiffre[i] == BACK_SLASH ) || ( (int) chiffre[i] == ZERO_TERMINAL ) || ( (int) chiffre[i] == ETOILE ) )
                good = FALSE ;
        }
    }

    // On envoie la réponse
    int longueur = strlen ( chiffre ) + 20 ;
    char* reponse = (char*) malloc ( longueur * sizeof ( char ) ) ;
    memset ( reponse, '\0', longueur ) ;
    sprintf ( reponse, "%d*%d*%s*EOF*", id, INIT, chiffre ) ;
    res_send ( reponse, strlen ( reponse ) ) ;

    // Free
    free ( chiffre ) ;
    free ( cle_aes ) ;
    free ( reponse ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
//
// Méthodes dépréciées
//
//----------------------------------------------------------


//----------------------------------------------------------
// int RSA_envoi_key_pub ()
//----------------------------------------------------------
// Permet d'envoyer notre clé RSA publique à la frontale

/*int RSA_envoi_key_pub ()
{
    // Déclaration variables
    int flen = 0 ;
    unsigned char* key = NULL ;
    BIO *keybio = NULL ;
    int taille ;
    char* reponse = NULL ;

    // Initialisation
    flen = RSA_size ( bdd_key ) + 170 ;
    key = (unsigned char*) malloc ( flen * sizeof ( unsigned char ) ) ;
    memset ( key, '\0', flen ) ;

    // On aloue notre object BIO avec la clé
    if ( ( keybio = BIO_new ( BIO_s_mem() ) ) == NULL )
    {
        perror ( "Erreur_RSA_envoi_key_pub : BIO_new() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        free ( key ) ;
        return ERRNO ;
    }

    // On écrit la clé dans le BIO
    if ( PEM_write_bio_RSAPublicKey ( keybio, bdd_key ) == 0 )
    {
        perror ( "Erreur_RSA_envoi_key_pub : PEM_write_bio_RSAPublicKey() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        free ( key ) ;
        BIO_free ( keybio ) ;
        return ERRNO ;
    }

    // On récupère la clé du BIO au format texte
    if ( BIO_read ( keybio, key, flen ) != flen )
    {
        perror ( "Erreur_RSA_envoi_key_pub : BIO_read() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        free ( key ) ;
        BIO_free ( keybio ) ;
        return ERRNO ;
    }

    // On free notre object BIO
    BIO_free ( keybio ) ;

    // On envoie la réponse
    taille = flen + 10 ;
    reponse = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( reponse, '\0', taille ) ;
    sprintf ( reponse, "%d*%s*EOF", INIT, key ) ;
    res_send ( reponse ) ;

    // Free
    free ( key ) ;
    free ( reponse ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}*/
