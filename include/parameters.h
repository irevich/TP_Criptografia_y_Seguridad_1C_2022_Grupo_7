#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__
#include <stdbool.h>
#include <stdint.h>

typedef enum stego_algorithm {
    LSB1,
    LSB4,
    LSBI
}stego_algorithm_t;

typedef enum encryption_algorithm {
    AES128,
    AES192,
    AES256,
    DES
} encryption_algorithm_t;

typedef enum encryption_mode {
    ECB,
    CFB,
    OFB,
    CBC
} encryption_mode_t;

typedef struct parameters{
    bool embed;
    char * input_file_path;
    char * carrier_file_path;
    char * output_file_path;
    stego_algorithm_t stego_algorithm;
    encryption_algorithm_t encryption_algorithm;
    encryption_mode_t encryption_mode;
    uint8_t * password;
} parameters_t;

/**
 * Interpreta la linea de comandos (argc, argv) llenando
 * args con defaults o la seleccion humana. Puede cortar
 * la ejecución.
 */
parameters_t * parse_args(const int argc, char **argv);

#endif