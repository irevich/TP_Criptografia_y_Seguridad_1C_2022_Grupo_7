#ifndef __STEGO_H__
#define __STEGO_H__

#include "bmp_parsing.h"
#include <string.h>
#include <stdlib.h>

bmp_file * lsb1_embed(bmp_file * carrier_bmp, char * source_file_path);
FILE * lsb1_extract(bmp_file * carrier_bmp, char * output_file_name);


#endif