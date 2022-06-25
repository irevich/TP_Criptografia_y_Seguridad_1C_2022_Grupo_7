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

    // // LSB1 EMBED
    // bmp_file * embed_bmp = lsb1_embed(carrier_bmp,parameters->input_file_path);
    // write_bmp_file(embed_bmp, "embed.bmp");

    // bmp_file * embed_bmp_from_file = read_bmp_file("embed.bmp");

    // LSB1 EXTRACT
    // lsb1_extract(carrier_bmp,parameters->output_file_path);

    // LSB4 EXTRACT
    //lsb4_extract(carrier_bmp,parameters->output_file_path);

    // LSBI EXTRACT
    lsbi_extract(carrier_bmp,parameters->output_file_path);

    //LSBI EMBED
    // bmp_file * output_bmp = lsbi_embed(carrier_bmp, parameters->input_file_path);
    // write_bmp_file(output_bmp, parameters->output_file_path);

    // Finally, we free the memory allocated
    // free(embed_bmp);
    free(carrier_bmp);
    
    return 0;
}


