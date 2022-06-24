#include <stdio.h>
#include <stdlib.h>
#include "include/bmp_parsing.h"
#include "include/parameters.h"
#include "include/stego.h"



int main(int argc, char *argv[]) {
    // Parse the parameters
    parameters_t * parameters = parse_args(argc, argv);    

    // parameters_t * hola;
    
    //Then, we read the bmp header
    bmp_file * carrier_bmp = read_bmp_file(parameters->carrier_file_path);

    if(carrier_bmp==NULL){
        fprintf(stderr, "Could not read the carrier file\n");
        exit(-1);
    }

    // write_bmp_file(carrier_bmp, "copia_carrier.bmp");

    // print_bmp_file(bmp_file);    
    // write_bmp_file(bmp_file, parameters->output_file_path);

    //lsb1 extract
    // lsb1_extract(bmp_file,parameters->output_file_path);

    bmp_file * output_bmp = lsb1_embed(carrier_bmp,parameters->input_file_path);

    write_bmp_file(output_bmp, parameters->output_file_path);


    // free(output_bmp);
    // free(carrier_bmp);
    return 0;
}