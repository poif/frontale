#include "crypt.h"
#include "default_lib.h"
//const static unsigned char aes_key[]={0x00,0x10,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
//const static unsigned char aes_key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88};
static unsigned char aes_key[AES_KEY_LENGTH+1];

RSA* frontale_key = NULL ;
RSA* client_key = NULL ;

/*======================================================================
**  Nom          : crypt
**  Description  : Permet le cryptage d'une chaine de caractere en aes
** ---------------------------------------------------------------------
**  Parametres   :
**      unsigned char * aes_input : chaine à crypter
**======================================================================*/

void crypt(unsigned char* aes_input, unsigned char *enc_out, int size_aes_input){
    
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);

    AES_KEY enc_key;
    AES_set_encrypt_key(aes_key, 256, &enc_key);
    AES_cbc_encrypt(aes_input, enc_out, size_aes_input, &enc_key, iv, AES_ENCRYPT);
    print_data("\n Encrypted",enc_out, strlen(aes_input)+16-(strlen(aes_input)%16));

 }

 /*======================================================================
**  Nom          : decrypt
**  Description  : Permet le decryptage d'une chaine cryptée en aes
** ---------------------------------------------------------------------
**  Parametres   :
**      unsigned char * enc_out : chaine à décrypter
**======================================================================*/

void decrypt(unsigned char* dec_in, unsigned char* dec_out , int size_aes_input){

    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);
    puts("Bonjour");
    AES_KEY dec_key;
    AES_set_decrypt_key(aes_key, 256, &dec_key);
    AES_cbc_encrypt(dec_in, dec_out, size_aes_input, &dec_key, iv, AES_DECRYPT);
    // print_data("\n Decrypted",dec_out, sizeof(dec_out));
    printf("%s\n", dec_out);
 }

 void print_data(const char *tittle, const void* data, int len)
{
    printf("%s : (%d)",tittle, len);
    const unsigned char * p = (const unsigned char*)data;
    int i = 0;
    
    for (; i<len; ++i)
        printf("%02X ", *p++);
    
    printf("\n");
}


int AES_generate_key ()
{
    int i ;

    memset ( aes_key, '\0', AES_KEY_LENGTH +1) ;

    for ( i = 0; i < AES_KEY_LENGTH - 1; i++ )
    {
        aes_key[i] = rand() % ( 122 - 97 ) + 97 ;
    }

    return 1 ;
}

int generate_RSA_keys ()
{
    // DEBUG
    printf ( "Generation cles RSA ... \n" ) ;

    // Déclaration variables
    BIGNUM *bne = NULL ;        // Exponent
    int bits = KEY_LENGTH ;     // Taille clé

    // On alloue notre object RSA
    if ( ( client_key = RSA_new() ) == NULL )
    {
        perror ( "Erreur_generate_RSA_keys : RSA_New() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return 0;
    }

    // On alloue notre object BIGNUM
    if ( ( bne = BN_new() ) == NULL )
    {
        perror ( "Erreur_generate_RSA_keys : BN_new() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return 0 ;
    }

    // On fixe l'exposant sur la valeur maximale courante
    if ( ( BN_set_word ( bne, 65537 ) ) == 0 )
    {
        perror ( "Erreur_generate_RSA_keys : BN_set_word() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return 0 ;
    }

    // On génère maintenant notre paire de clés RSA
    if ( ( RSA_generate_key_ex ( client_key, bits, bne, NULL ) ) != 1 )
    {
        perror ( "Erreur_generate_RSA_keys : RSA_generate_key_ex() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return 0 ;
    }

    // On free notre object BIGNUM
    BN_free ( bne ) ;

    // Dernière vérification
    if ( client_key == NULL )
    {
        perror ( "Erreur_generate_RSA_keys : impossible de generer les cles " ) ;
        return 0 ;
    }
    else{
        
        return 1 ;
    }
}

int recup_aes(char *crypte_aes)
{
    return RSA_chiffrement(crypte_aes, aes_key, DECHIFFRE);
}

int RSA_chiffrement ( unsigned char* input, unsigned char* output, int mode )
{
    // Initialisation
    int flen = RSA_size ( client_key ) - PADDING_FLEN ;

    // On commence par vérifier l'intégrité de notre paire de clés
    if ( RSA_check_key ( client_key ) != 1 )
    {
        perror ( "Erreur_RSA_chiffrement : invalid RSA keys " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return 0 ;
    }

    if ( mode == CHIFFRE )
    {
        if ( ( RSA_public_encrypt ( flen , input, output, client_key, RSA_PKCS1_PADDING ) ) <= 0 )
        {
            perror ( "Erreur_RSA_chiffrement : error chiffrement " ) ;
            fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
            return 0 ;
        }
    }
    else if ( mode == DECHIFFRE )
    {
        if ( ( RSA_private_decrypt ( flen + PADDING_FLEN, input, output, client_key, RSA_PKCS1_PADDING) ) <= 0 )
        {
            perror ( "Erreur_RSA_dechiffrement : error déchiffrement " ) ;
            fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
            return 0 ;
        }
    }
    else
    {
        perror ( "Erreur_RSA_chiffrement : bad mode " ) ;
        return 0 ;
    }

    // On indique que tout s'est bien déroulé
    printf("je retourne ici\n");
    aes_key[AES_KEY_LENGTH] = '\0';
    return 1 ;
}

int RSA_envoi_key_pub ()
{
    // Déclaration variables
    int flen = 0 ;
    unsigned char* key = NULL ;
    BIO *keybio = NULL ;
    int taille ;
    char* reponse = NULL ;

    // Initialisation
    flen = RSA_size ( client_key ) + 170 ;
    key = (unsigned char*) malloc ( flen * sizeof ( unsigned char ) ) ;
    memset ( key, '\0', flen ) ;

    // On aloue notre object BIO avec la clé
    if ( ( keybio = BIO_new ( BIO_s_mem() ) ) == NULL )
    {
        perror ( "Erreur_RSA_envoi_key_pub : BIO_new() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        free ( key ) ;
        return 0 ;
    }

    // On écrit la clé dans le BIO
    if ( PEM_write_bio_RSAPublicKey ( keybio, client_key ) == 0 )
    {
        perror ( "Erreur_RSA_envoi_key_pub : PEM_write_bio_RSAPublicKey() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        free ( key ) ;
        BIO_free ( keybio ) ;
        return 0 ;
    }

    // On récupère la clé du BIO au format texte
    if ( BIO_read ( keybio, key, flen ) != flen )
    {
        perror ( "Erreur_RSA_envoi_key_pub : BIO_read() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        free ( key ) ;
        BIO_free ( keybio ) ;
        return 0 ;
    }

    // On free notre object BIO
    BIO_free ( keybio ) ;

    // On envoie la réponse
    taille = flen + 10 ;
    reponse = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( reponse, '\0', taille ) ;
    sprintf ( reponse, "init*%s*EOF", key ) ;
    envoi_requete ( reponse , strlen(reponse), WANT) ;

    // Free
    free ( key ) ;
    free ( reponse ) ;

    // On indique que tout s'est bien déroulé
    return 1 ;
}

int RSA_recup_key_pub ( unsigned char* key )
{
    // Déclaration variables
    BIO *keybio = NULL ;

    // On aloue notre object BIO avec la clé
    if ( ( keybio = BIO_new_mem_buf ( key, -1 ) ) == NULL )
    {
        perror ( "Erreur_RSA_recup_key_pub : BIO_new_mem_buf() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return 0 ;
    }

    // On lie la clé à la structure RSA
    if ( ( frontale_key = PEM_read_bio_RSAPublicKey ( keybio, &frontale_key, NULL, NULL ) ) == NULL )
    {
        perror ( "Erreur_RSA_recup_key_pub : PEM_read_bio_RSA_PUBKEY() " ) ;
        fprintf ( stderr, "Code erreur OpenSSL : %lu \n ", ERR_get_error () ) ;
        return 0 ;
    }

    // On free notre object BIO
    BIO_free ( keybio ) ;

    // On indique que tout s'est bien déroulé
    return 1 ;
}
