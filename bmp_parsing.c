#include "bmp_parsing.h"

bmp_file * read_bmp_file(char * filepath){
   //Allocate memory for header
   bmp_header * header = ((bmp_header*) malloc(sizeof(bmp_header)));
   bmp_info_header * info_header = ((bmp_info_header*) malloc(sizeof(bmp_info_header)));
   
    //First, we read bmp file
    FILE * fp = fopen(filepath,"rb");
    if(fp == NULL){
        return NULL;
    }

   ReadUShort(fp,header->type,FALSE);
   fprintf(stderr,"ID is: %d, should be %d\n",header->type,'M'*256+'B');
   ReadUInt(fp,header->file_size,FALSE);
   fprintf(stderr,"File size is %d bytes\n",header->file_size);
   ReadUShort(fp,header->reserved1,FALSE);
   ReadUShort(fp,header->reserved2,FALSE);
   ReadUInt(fp,header->offset,FALSE);
   fprintf(stderr,"Offset to image data is %d bytes\n",header->offset);
   
   //infoheader section
   //Read and check the information header
   if (fread(info_header,sizeof(bmp_info_header),1,fp) != 1) {
      fprintf(stderr,"Failed to read BMP info header\n");
      exit(-1);
   }
   fprintf(stderr,"Image size = %d x %d\n",info_header->width,info_header->height);
   fprintf(stderr,"Number of colour planes is %d\n",info_header->planes);
   fprintf(stderr,"Bits per pixel is %d\n",info_header->bits);
   fprintf(stderr,"Compression type is %d\n",info_header->compression);
   fprintf(stderr,"Number of colours is %d\n",info_header->ncolors);
   fprintf(stderr,"Number of required colours is %d\n",info_header->important_colors);


   //  fread(header,1,sizeof(bmp_header),fp);

    //Check if file is a bitmap
    if(header->type != 'M'*256+'B'){
        fclose(fp);
        return NULL;
    }

    //Then, we read the body

    //Move the file pointer when the data starts
    fseek(fp, header->offset, SEEK_SET);


    char * body = (char*) malloc(info_header->image_size);
    fread(body,info_header->image_size,1,fp);

    //Finally, create a bmp file with header and body data
    bmp_file * file = (bmp_file*) malloc(info_header->header_size + info_header->image_size);
    file->header = header;
    file->info_header = info_header;
    file->body = body;

    fclose(fp);

    return file;

}

void print_bmp_file(bmp_file * bmp_file){
   printf("BMP PROPERTIES:\n");
   printf("\t-Type:%x\n",bmp_file->header->type);
   printf("\t-File size:%d\n",(bmp_file->header)->file_size);
   printf("\t-Reserved1:%d\n",(bmp_file->header)->reserved1);
   printf("\t-Reserved2:%d\n",(bmp_file->header)->reserved2);
   printf("\t-Data offset:%d\n",(bmp_file->header)->offset);
   printf("\t-Header size:%d\n",(bmp_file->info_header)->header_size);
   printf("\t-Width:%d\n",(bmp_file->info_header)->width);
   printf("\t-Height:%d\n",(bmp_file->info_header)->height);
   printf("\t-Planes numbers:%x\n",bmp_file->info_header->planes);
   // printf("\t-Point size:%c%c\n",bmp_file->header->point_size[0],bmp_file->header->point_size[1]);
   // printf("\t-Compression:%d\n",(bmp_file->header)->compression);
   // printf("\t-Image size:%d\n",(bmp_file->header)->image_size);
   // printf("\t-Horizontal resolution:%d\n",(bmp_file->header)->horizontal_resolution);
   // printf("\t-Vertical resolution:%d\n",(bmp_file->header)->vertical_resolution);
   // printf("\t-Color table size:%d\n",(bmp_file->header)->color_table_size);
   // printf("\t-Important colors counter:%d\n",(bmp_file->header)->important_colors_counter);
}

int ReadUShort(FILE *fptr,short unsigned *n,int swap)
{
   unsigned char *cptr,tmp;

   if (fread(n,2,1,fptr) != 1)
      return(FALSE);
   if (swap) {
      cptr = (unsigned char *)n;
      tmp = cptr[0];
      cptr[0] = cptr[1];
      cptr[1] =tmp;
   }
   return(TRUE);
}

/*
   Read a possibly byte swapped unsigned integer
*/
int ReadUInt(FILE *fptr,unsigned int *n,int swap)
{
   unsigned char *cptr,tmp;

   if (fread(n,4,1,fptr) != 1)
      return(FALSE);
   if (swap) {
      cptr = (unsigned char *)n;
      tmp = cptr[0];
      cptr[0] = cptr[3];
      cptr[3] = tmp;
      tmp = cptr[1];
      cptr[1] = cptr[2];
      cptr[2] = tmp;
   }
   return(TRUE);
}
