
#include "encription.h"

const EVP_CIPHER * (*encryption_algorithm_functions[16])(void)= {EVP_aes_128_ecb,EVP_aes_128_cfb,EVP_aes_128_ofb,EVP_aes_128_cbc,EVP_aes_192_ecb,EVP_aes_192_cfb,EVP_aes_192_ofb,EVP_aes_192_cbc,EVP_aes_256_ecb,EVP_aes_256_cfb,EVP_aes_256_ofb,EVP_aes_256_cbc,EVP_des_ecb,EVP_des_cfb,EVP_des_ofb,EVP_des_cbc,
};

int encrypt(char algorithm,char mode, unsigned char *plaintext, int plaintext_len, unsigned char *key,unsigned char *iv, unsigned char *ciphertext) {
    int option_index = algorithm*4 + mode;
    
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        printf("Error encrypting");

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    // if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    //     printf("Error encrypting");
    if(1 != EVP_EncryptInit_ex(ctx, (*encryption_algorithm_functions[option_index])(), NULL, key, iv))
        printf("Error encrypting");

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        printf("Error encrypting");
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        printf("Error encrypting");
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(char algorithm,char mode,unsigned char *ciphertext, int ciphertext_len, unsigned char *key,unsigned char *iv, unsigned char *plaintext) {
        int option_index = algorithm*4 + mode;
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        printf("Error encrypting");

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx,(*encryption_algorithm_functions[option_index])(), NULL, key, iv))
        printf("Error encrypting");

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        printf("Error encrypting");
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        printf("Error encrypting");
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}


//EXAMPLE OF ENCRIPTION/DECRYPTION USE:

// int main (void)
// {
//     /*
//      * Set up the key and iv. Do I need to say to not hard code these in a
//      * real application? :-)
//      */

//     /* A 256 bit key */
//     unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
//     //unsigned char *key = (unsigned char *)"hola";

//     /* A 128 bit IV */
//     unsigned char *iv = (unsigned char *)"0123456789012345";

//     /* Message to be encrypted */
//     unsigned char *plaintext =
//         (unsigned char *)"The quick brown fox jumps over the lazy dog";

//     /*
//      * Buffer for ciphertext. Ensure the buffer is long enough for the
//      * ciphertext which may be longer than the plaintext, depending on the
//      * algorithm and mode.#include <stdio.h>
//      */
//     unsigned char ciphertext[128];

//     /* Buffer for the decrypted text */
//     unsigned char decryptedtext[128];

//     int decryptedtext_len, ciphertext_len;

//     /* Encrypt the plaintext */
//     ciphertext_len = encrypt (0,1, plaintext, strlen ((char *)plaintext), key, iv,
//                               ciphertext);

//     /* Do something useful with the ciphertext here */
//     printf("Ciphertext is:\n");
//     BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

//     /* Decrypt the ciphertext */
//     decryptedtext_len = decrypt(0,1,ciphertext, ciphertext_len, key, iv,
//                                 decryptedtext);

//     /* Add a NULL terminator. We are expecting printable text */
//     decryptedtext[decryptedtext_len] = '\0';

//     /* Show the decrypted text */
//     printf("Decrypted text is:\n");
//     printf("%s\n", decryptedtext);


//     return 0;
// }