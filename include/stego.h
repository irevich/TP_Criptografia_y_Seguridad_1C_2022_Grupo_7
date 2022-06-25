#ifndef __STEGO_H__
#define __STEGO_H__

#include "bmp_parsing.h"
#include "parameters.h"
#include <string.h>
#include <stdlib.h>

bmp_file * embed(stego_algorithm_t stego,bmp_file * carrier_bmp, char * input_file_path);
FILE * extract(stego_algorithm_t stego,bmp_file * carrier_bmp, char * output_file_name);

#endif