#include "bmp_parsing.h"

bmp_file * read_bmp_file(char * filepath){
    //First, we read bmp file
    FILE * fp = fopen(filepath,"rb");
    if(fp == NULL){
        return NULL;
    }

    //Then, we read the bmp header
    bmp_header * header = (bmp_header*) malloc(sizeof(bmp_header));

    fread(header,1,sizeof(bmp_header),fp);

    //Check if file is a bitmap
    if(header->type[0]!='B' || header->type[1]!='M'){
        fclose(fp);
        return NULL;
    }

    //Then, we read the body

    //Move the file pointer when the data starts
    fseek(fp, header->data_offset, SEEK_SET);
    char * body = (char*) malloc(header->image_size);
    fread(body,header->image_size,1,fp);

    //Finally, create a bmp file with header and body data
    bmp_file * file = (bmp_file*) malloc(header->header_size + header->image_size);
    file->header = header;
    file->body = body;

    fclose(fp);

    return file;

}

void print_bmp_file(bmp_file * bmp_file){
    printf("BMP PROPERTIES:\n");
    printf("\t-Type:%c%c\n",bmp_file->header->type[0],bmp_file->header->type[1]);
    printf("\t-File size:%d\n",(bmp_file->header)->file_size);
    printf("\t-Reservated:%d\n",(bmp_file->header)->reservated);
    printf("\t-Data offset:%d\n",(bmp_file->header)->data_offset);
    printf("\t-Header size:%d\n",(bmp_file->header)->header_size);
    printf("\t-Width:%d\n",(bmp_file->header)->width);
    printf("\t-Height:%d\n",(bmp_file->header)->height);
    printf("\t-Planes numbers:%c%c\n",bmp_file->header->planes_number[0],bmp_file->header->planes_number[1]);
    printf("\t-Point size:%c%c\n",bmp_file->header->point_size[0],bmp_file->header->point_size[1]);
    printf("\t-Compression:%d\n",(bmp_file->header)->compression);
    printf("\t-Image size:%d\n",(bmp_file->header)->image_size);
    printf("\t-Horizontal resolution:%d\n",(bmp_file->header)->horizontal_resolution);
    printf("\t-Vertical resolution:%d\n",(bmp_file->header)->vertical_resolution);
    printf("\t-Color table size:%d\n",(bmp_file->header)->color_table_size);
    printf("\t-Important colors counter:%d\n",(bmp_file->header)->important_colors_counter);
}
