#ifndef __ENCRIPTION_H_

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

int decrypt(char algorithm,char mode,unsigned char *ciphertext, int ciphertext_len, unsigned char *key,    unsigned char *iv, unsigned char *plaintext);

int encrypt(char algorithm,char mode,unsigned char *plaintext, int plaintext_len, unsigned char *key,unsigned char *iv, unsigned char *ciphertext);

#endif
