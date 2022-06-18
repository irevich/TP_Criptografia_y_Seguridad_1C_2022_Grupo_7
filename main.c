#include <stdio.h>
#include <stdlib.h>
#include "bmp_parsing.h"
#include "encription.h"

int main(int argc, char *argv[]){
    char * bmp_filepath = "./archivos/back.bmp";

    //Then, we read the bmp header
    bmp_file * bmp_file = read_bmp_file(bmp_filepath);
    print_bmp_file(bmp_file);    
    free(bmp_file);
    return 0;
}