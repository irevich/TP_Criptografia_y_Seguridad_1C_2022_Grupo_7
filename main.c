#include <stdio.h>
#include <stdlib.h>
#include "include/bmp_parsing.h"
#include "include/parameters.h"
#include "include/stego.h"
#include "include/encription.h"
// #include <time.h>


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
        // clock_t begin = clock();
        bmp_file * embed_bmp = embed(parameters->stego_algorithm,carrier_bmp,parameters->input_file_path,parameters->encryption_algorithm,parameters->encryption_mode,parameters->password);
        write_bmp_file(embed_bmp, parameters->output_file_path);
        // clock_t end = clock();
        // double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        // int bits_difference = get_bmp_body_bit_difference(carrier_bmp,embed_bmp);
        // int bytes_difference = get_bmp_body_byte_difference(carrier_bmp,embed_bmp);
        // printf("Execution time: %f\n",time_spent);
        // printf("Bits difference with original bmp: %d\n",bits_difference);
        // printf("Bytes difference with original bmp: %d\n",bytes_difference);
    }else{
        extract(parameters->stego_algorithm,carrier_bmp,parameters->output_file_path,parameters->encryption_algorithm,parameters->encryption_mode,parameters->password);
    }

    // Finally, we free the memory allocated
    //free(embed_bmp);
    free(carrier_bmp);
    
    return 0;
}


