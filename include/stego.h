#ifndef __STEGO_H__
#define __STEGO_H__

#include "bmp_parsing.h"
#include "parameters.h"
#include <string.h>
#include <stdlib.h>

bmp_file * embed(stego_algorithm_t stego,bmp_file * carrier_bmp, char * input_file_path,int encryption_algorithm, int encryption_mode, char * password);
FILE * extract(stego_algorithm_t stego,bmp_file * carrier_bmp, char * output_file_name,int encryption_algorithm, int encryption_mode, char * password);

//With encryption

bmp_file * lsb1_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, int encryption_algorithm, int encryption_mode, char * password);
FILE * lsb1_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, int encryption_algorithm, int encryption_mode, char * password);

bmp_file * lsb4_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, int encryption_algorithm, int encryption_mode, char * password);
FILE * lsb4_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, int encryption_algorithm, int encryption_mode, char * password);

bmp_file * lsbi_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, int encryption_algorithm, int encryption_mode, char * password);
FILE * lsbi_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, int encryption_algorithm, int encryption_mode, char * password);


#endif