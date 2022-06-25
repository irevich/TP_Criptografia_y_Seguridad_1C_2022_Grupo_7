#ifndef __STEGO_H__
#define __STEGO_H__

#include "bmp_parsing.h"
#include <string.h>
#include <stdlib.h>

bmp_file * lsb1_embed(bmp_file * carrier_bmp, char * source_file_path);
FILE * lsb1_extract(bmp_file * carrier_bmp, char * output_file_name);

bmp_file * lsb4_embed(bmp_file * carrier_bmp, char * source_file_path);
FILE * lsb4_extract(bmp_file * carrier_bmp, char * output_file_name);

bmp_file * lsbi_embed(bmp_file * carrier_bmp, char * source_file_path);
FILE * lsbi_extract(bmp_file * carrier_bmp, char * output_file_name);

#endif