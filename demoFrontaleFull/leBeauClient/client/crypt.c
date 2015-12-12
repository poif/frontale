#include "crypt.h"

const static unsigned char aes_key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88};


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
    printf("Reponse : %s\n", dec_out);
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