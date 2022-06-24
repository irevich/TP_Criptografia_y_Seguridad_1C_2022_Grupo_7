#ifndef __BMP_PARSING_H_
#define __BMP_PARSING_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define FALSE 0
#define TRUE 1

typedef struct pixel{
    uint8_t colors[3]; //Blue,green and red
}pixel;

typedef struct bmp_info_header{
    uint32_t header_size;                   //Header size in bytes
    uint32_t width;                         //Width of the image
    uint32_t height;                        //Height of the image
    uint16_t planes;                        //Number of color planes
    uint16_t bits;                          //Bits per pixel
    uint32_t compression;                   //Compression type
    uint32_t image_size;                    //Image size in bytes     
    uint32_t x_resolution,y_resolution;     //Pixels per meter
    uint32_t ncolors;                       //Number of colours
    uint32_t important_colors;              //Important colours
}bmp_info_header;


typedef struct bmp_header{
    uint16_t type;          //Magic identifier
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;        //Offset to image data, bytes
    uint32_t file_size;     //File size in bytes

}bmp_header;

typedef struct bmp_file{
    bmp_header * header;
    bmp_info_header * info_header;
     pixel * body;
    // uint8_t * body;
}bmp_file;

bmp_file * read_bmp_file(char * filepath);

void print_bmp_file(bmp_file * bmp_file);

int ReadUShort(FILE *fptr,short unsigned *n,int swap);

/*
   Read a possibly byte swapped uint32_teger
*/
int ReadUInt(FILE *fptr,uint32_t *n,int swap);

// Writes the bmp file {file} in the path {filepath}
void write_bmp_file(bmp_file * file, char * filepath);


#endif