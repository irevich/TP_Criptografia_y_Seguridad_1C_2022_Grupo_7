#include <stdio.h>
#include <stdlib.h>
#include "include/bmp_parsing.h"
#include "include/parameters.h"
#include "include/stego.h"



int main(int argc, char *argv[]) {
    // Parse the parameters
    parameters_t * parameters = parse_args(argc, argv);  
    
    // Then, we read the carrier bmp
    bmp_file * carrier_bmp = read_bmp_file(parameters->carrier_file_path);

    if(carrier_bmp==NULL){
        fprintf(stderr, "Could not read the carrier file\n");
        exit(-1);
    }

    // Then, we generate the output bmp as result of lsb1 embed method
    // bmp_file * output_bmp = lsb1_embed(carrier_bmp,parameters->input_file_path);
    // write_bmp_file(output_bmp, parameters->output_file_path);
    
    bmp_file * output_bmp = lsbi_embed(carrier_bmp, parameters->input_file_path);
    write_bmp_file(output_bmp, parameters->output_file_path);
    // Finally, we free the memory allocated
    free(output_bmp);
    free(carrier_bmp);
    
    return 0;
}
