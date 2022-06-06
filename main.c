#include <stdio.h>
#include <stdlib.h>
#include "bmp_parsing.h"

int main(int argc, char *argv[]) {

    char * bmp_filepath = "back.bmp";
    bmp_file * bmp_file = read_bmp_file(bmp_filepath);
    print_bmp_file(bmp_file);

    return 0;
}