#ifndef CRYPT_H
#define CRYPT_H

#include "default_lib.h"

#define CHIFFRE 1
#define DECHIFFRE 2

void crypt(unsigned char* aes_input, unsigned char *enc_out, int size_aes_input);
void decrypt(unsigned char* dec_in, unsigned char* dec_out, int size_aes_input);

 void print_data(const char *tittle, const void* data, int len);


#endif