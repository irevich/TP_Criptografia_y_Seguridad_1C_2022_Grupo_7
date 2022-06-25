#ifndef __STEGO_H__
#define __STEGO_H__

#include "bmp_parsing.h"
#include "parameters.h"
#include <string.h>
#include <stdlib.h>

bmp_file * embed(stego_algorithm_t stego,bmp_file * carrier_bmp, char * input_file_path,encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);
FILE * extract(stego_algorithm_t stego,bmp_file * carrier_bmp, char * output_file_name,encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);


//With encryption

bmp_file * lsb1_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);
FILE * lsb1_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);

bmp_file * lsb4_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);
FILE * lsb4_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);

bmp_file * lsbi_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);
FILE * lsbi_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password);


#endif