#include <stdio.h>
#include <stdlib.h>
#include "include/bmp_parsing.h"
#include "include/parameters.h"
#include "include/stego.h"
#include "include/encription.h"


int main(int argc, char *argv[]) {
    // Parse the parameters
    parameters_t * parameters = parse_args(argc, argv);  
    
    // Then, we read the carrier bmp
    bmp_file * carrier_bmp = read_bmp_file(parameters->carrier_file_path);

    if(carrier_bmp==NULL){
        fprintf(stderr, "Could not read the carrier file\n");
        exit(-1);
    }
    if(parameters->embed){
        bmp_file * embed_bmp = embed(carrier_bmp, parameters);
        free(embed_bmp);
    }else{
        extract(parameters->stego_algorithm,carrier_bmp,parameters->output_file_path);
    }

    // lsb1_extract_with_encryption(carrier_bmp,parameters->output_file_path,parameters->encryption_algorithm,parameters->encryption_mode,parameters->password);

    // Finally, we free the memory allocated
    free(carrier_bmp);
    
    return 0;
}


