#include "traitement_commande.h"

/* AES key for Encryption and Decryption */
const static unsigned char aes_key[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

/*======================================================================
**  Nom          : crypt
**  Description  : Permet le cryptage d'une chaine de caractere en aes
** ---------------------------------------------------------------------
**  Parametres   :
**      unsigned char * aes_input : chaine à crypter
**======================================================================*/
  
void crypt(unsigned char* aes_input, unsigned char *enc_out){
    
    /* Init vector */
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);
    
    /* Buffers for Encryption and Decryption */
    //unsigned char enc_out[sizeof(aes_input)];

    /* AES-128 bit CBC Encryption */
    AES_KEY enc_key;
    AES_set_encrypt_key(aes_key, sizeof(aes_key)*8, &enc_key);
    AES_cbc_encrypt(aes_input, enc_out, sizeof(aes_input), &enc_key, iv, AES_ENCRYPT);

    printf("%s\n", enc_out);

 }

 /*======================================================================
**  Nom          : decrypt
**  Description  : Permet le decryptage d'une chaine cryptée en aes
** ---------------------------------------------------------------------
**  Parametres   :
**      unsigned char * enc_out : chaine à décrypter
**======================================================================*/

void decrypt(unsigned char* dec_in, unsigned char* dec_out){

    /* Init vector */
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0x00, AES_BLOCK_SIZE);
    
    /* Buffer for Decryption */
    //unsigned char dec_out[sizeof(dec_in)];
    
    /* AES-128 bit CBC Decryption */
    AES_KEY dec_key;
    AES_set_decrypt_key(aes_key, sizeof(aes_key)*8, &dec_key); // Size of key is in bits
    AES_cbc_encrypt(dec_in, dec_out, sizeof(dec_in), &dec_key, iv, AES_DECRYPT);
    

    printf("%s\n", dec_out);
 }