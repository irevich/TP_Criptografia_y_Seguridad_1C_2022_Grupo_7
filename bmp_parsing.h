#ifndef __BMP_PARSING_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct bmp_header{
    char type[2];             //2 bytes
    unsigned int file_size;   //4 bytes
    unsigned int reservated;  //4 bytes
    unsigned int data_offset; //4 bytes
    unsigned int header_size; //4 bytes
    unsigned int width;       //4 bytes
    unsigned int height;      //4 bytes
    char planes_number[2];    //2 bytes
    char point_size[2];        //2 bytes
    unsigned int compression;  //4 bytes
    unsigned int image_size;  //4 bytes
    unsigned int horizontal_resolution;  //4 bytes
    unsigned int vertical_resolution;  //4 bytes
    unsigned int color_table_size;  //4 bytes
    unsigned int important_colors_counter;  //4 bytes

}bmp_header;

typedef struct bmp_file
{
    bmp_header * header;
    char * body;
}bmp_file;

bmp_file * read_bmp_file(char * filepath);

void print_bmp_file(bmp_file * bmp_file);


#endif