#include "crypt.h"

const static unsigned char aes_key[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};


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
    AES_set_encrypt_key(aes_key, sizeof(aes_key)*8, &enc_key);
    AES_cbc_encrypt(aes_input, enc_out, size_aes_input, &enc_key, iv, AES_ENCRYPT);
    print_data("\n Encrypted",enc_out, strlen(enc_out));

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
    
    AES_KEY dec_key;
    AES_set_decrypt_key(aes_key, sizeof(aes_key)*8, &dec_key);
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