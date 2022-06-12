#include <stdio.h>
#include <stdlib.h>
#include "include/bmp_parsing.h"
#include "include/parameters.h"

int main(int argc, char *argv[]) {
    char * bmp_filepath = "resources/lado.bmp";
    // Parse the parameters
    parameters_t * parameters = parse_args(argc, argv);    
    printf("Lei los parámetros\n");

    //Then, we read the bmp header
    bmp_file * bmp_file = read_bmp_file(parameters->carrier_file_path);
    print_bmp_file(bmp_file);    
    free(bmp_file);
    return 0;
}