#ifndef __ENCRIPTION_H_

#include "parameters.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

int decrypt(encryption_algorithm_t algorithm,encryption_mode_t mode,unsigned char *ciphertext, int ciphertext_len, unsigned char *password, unsigned char *plaintext);

int encrypt(encryption_algorithm_t algorithm,encryption_mode_t mode,unsigned char *plaintext, int plaintext_len, unsigned char *password, unsigned char *ciphertext);

#endif
