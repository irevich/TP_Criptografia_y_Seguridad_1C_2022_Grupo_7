#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__
#include <stdbool.h>

struct parameters{
    bool embed;
    char * input_file_path;
    char * carrier_file_path;
    char * output_file_path;
    int stego_algorithm;
    int encryption_algorithm;
    int encryption_mode;
    char * password;
};

enum stego_algorithm {
    LSB1,
    LSB4,
    LSBI
};

enum encryption_algorithm {
    AES128,
    AES192,
    AES256,
    DES
};

enum encryption_mode {
    ECB,
    CFB,
    OFB,
    CBC
};

/**
 * Interpreta la linea de comandos (argc, argv) llenando
 * args con defaults o la seleccion humana. Puede cortar
 * la ejecución.
 */
void parse_args(const int argc, char **argv, struct parameters *args);

#endif