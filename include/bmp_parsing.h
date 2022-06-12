#ifndef __BMP_PARSING_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define FALSE 0
#define TRUE 1

typedef struct bmp_info_header{
    unsigned int header_size;   //Header size in bytes
    int width;                  //Width of the image
    int height;                 //Height of the image
    unsigned short int planes;  //Number of color planes
    unsigned short int bits;    //Bits per pixel
    unsigned int compression;   //Compression type
    unsigned int image_size;    //Image size in bytes     
    int x_resolution,y_resolution;  //Pixels per meter
    unsigned int ncolors;          //Number of colours
    unsigned int important_colors;  //Important colours
}bmp_info_header;


typedef struct bmp_header{
    unsigned short int type;    //Magic identifier
    unsigned int file_size;     //File size in bytes
    unsigned short int reserved1;
    unsigned short int reserved2;
    unsigned int offset;        //Offset to image data, bytes
}bmp_header;

typedef struct bmp_file{
    bmp_header * header;
    bmp_info_header * info_header;
    uint8_t * body;
}bmp_file;

bmp_file * read_bmp_file(char * filepath);

void print_bmp_file(bmp_file * bmp_file);

int ReadUShort(FILE *fptr,short unsigned *n,int swap);


/*
   Read a possibly byte swapped unsigned integer
*/
int ReadUInt(FILE *fptr,unsigned int *n,int swap);

#endif