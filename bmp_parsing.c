#include "include/bmp_parsing.h"

bmp_file *read_bmp_file(char *filepath)
{

   // First, we read bmp file
   FILE *fp = fopen(filepath, "rb");
   if (fp == NULL){
      return NULL;
   }

   // Allocate memory for header
   bmp_header *header = ((bmp_header *)malloc(sizeof(bmp_header)));   

   ReadUShort(fp, &(header->type), FALSE);
   ReadUInt(fp, &(header->file_size), FALSE);
   ReadUShort(fp, &(header->reserved1), FALSE);
   ReadUShort(fp, &(header->reserved2), FALSE);
   ReadUInt(fp, &(header->offset), FALSE);

   fprintf(stderr, "ID is: %d and it should be %d\n", header->type, 'M' * 256 + 'B');
   fprintf(stderr, "File size is %d bytes\n", header->file_size);
   fprintf(stderr, "Offset to image data is %d bytes\n", header->offset);

   // Allocate memory for infoheader
   bmp_info_header *info_header = ((bmp_info_header *)malloc(sizeof(bmp_info_header)));
   // Read and check the information header
   if (fread(info_header, sizeof(bmp_info_header), 1, fp) != 1)
   {
      fprintf(stderr, "Failed to read BMP info header\n");
      exit(-1);
   }

   // Check compression
   if (info_header->compression!=0)
   {
      fprintf(stderr, "BMP must not be compressed\n");
      exit(-1);
   }
   
   fprintf(stderr, "Image size = %d x %d\n", info_header->width, info_header->height);
   fprintf(stderr, "Number of colour planes is %d\n", info_header->planes);
   fprintf(stderr, "Bits per pixel is %d\n", info_header->bits);
   fprintf(stderr, "Compression type is %d\n", info_header->compression);
   fprintf(stderr, "Number of colours is %d\n", info_header->ncolors);
   fprintf(stderr, "Number of required colours is %d\n", info_header->important_colors);


   // Check if file is a bitmap
   if (header->type != 'M' * 256 + 'B')
   {
      fprintf(stderr, "File must be a Bitmap\n");
      exit(-1);
   }

   // Then, we read the body

   // Move the file pointer when the data starts
   fseek(fp, header->offset, SEEK_SET);

   pixel *body = (pixel *)malloc(sizeof(pixel) * (info_header->width) * (info_header->height));

   // uint8_t *body = (uint8_t *)malloc(sizeof(uint8_t) * (info_header->width) * (info_header->height) * 3);

   int i, j, k;
   // int body_index = 0;
   for (j = 0; j < info_header->height; j++)
   {
      for (i = 0; i < info_header->width; i++)
      {
         uint8_t color;
         for (k = 0; k < 3; k++)
         {
            if (fread(&color, sizeof(uint8_t), 1, fp) != 1)
            {
               fprintf(stderr, "Failed to read BMP body\n");
               exit(-1);
            }

            body[j * info_header->width + i].colors[k] = color;
            // body_index++;
         }
      }
   }

   // Finally, create a bmp file with header and body data
   bmp_file *file = (bmp_file *)malloc(info_header->header_size + info_header->image_size);
   file->header = header;
   file->info_header = info_header;
   file->body = body;

   fclose(fp);

   return file;
}

void write_bmp_file(bmp_file * file, char * filepath){
   FILE *fp = fopen(filepath, "wb");
   fwrite(&(file->header->type), sizeof(uint16_t), 1, fp);
   fwrite(&(file->header->file_size), sizeof(uint32_t), 1, fp);
   fwrite(&(file->header->reserved1), sizeof(uint16_t), 1, fp);
   fwrite(&(file->header->reserved2), sizeof(uint16_t), 1, fp);
   fwrite(&(file->header->offset), sizeof(uint32_t), 1, fp);

   fwrite(file->info_header, sizeof(bmp_info_header), 1, fp);
   // fwrite(file->body, sizeof(uint8_t), file->info_header->width * file->info_header->height * 3, fp);
   fwrite(file->body, sizeof(pixel), file->info_header->width * file->info_header->height, fp);
   fclose(fp);
}

void print_bmp_file(bmp_file *bmp_file)
{
   printf("BMP PROPERTIES:\n");
   printf("\t-Type:%x\n", bmp_file->header->type);
   printf("\t-File size:%d\n", (bmp_file->header)->file_size);
   printf("\t-Reserved1:%d\n", (bmp_file->header)->reserved1);
   printf("\t-Reserved2:%d\n", (bmp_file->header)->reserved2);
   printf("\t-Data offset:%d\n", (bmp_file->header)->offset);
   printf("\t-Header size:%d\n", (bmp_file->info_header)->header_size);
   printf("\t-Width:%d\n", (bmp_file->info_header)->width);
   printf("\t-Height:%d\n", (bmp_file->info_header)->height);
   printf("\t-Planes numbers:%x\n", bmp_file->info_header->planes);
   printf("\t-Bits:%x\n", bmp_file->info_header->bits);
   printf("\t-Compression:%d\n", bmp_file->info_header->compression);
   printf("\t-Image size:%d\n", bmp_file->info_header->image_size);
   printf("\t-X resolution:%d\n", bmp_file->info_header->x_resolution);
   printf("\t-Y resolution:%d\n", bmp_file->info_header->y_resolution);
   printf("\t-N colors:%d\n", bmp_file->info_header->ncolors);
   printf("\t-Important colors:%d\n", bmp_file->info_header->important_colors);
}

int ReadUShort(FILE *fptr, short unsigned *n, int swap)
{
   unsigned char *cptr, tmp;

   if (fread(n, 2, 1, fptr) != 1)
      return (FALSE);
   if (swap)
   {
      cptr = (unsigned char *)n;
      tmp = cptr[0];
      cptr[0] = cptr[1];
      cptr[1] = tmp;
   }
   return (TRUE);
}

/*
   Read a possibly byte swapped unsigned integer
*/
int ReadUInt(FILE *fptr, unsigned int *n, int swap)
{
   unsigned char *cptr, tmp;

   if (fread(n, 4, 1, fptr) != 1)
      return (FALSE);
   if (swap)
   {
      cptr = (unsigned char *)n;
      tmp = cptr[0];
      cptr[0] = cptr[3];
      cptr[3] = tmp;
      tmp = cptr[1];
      cptr[1] = cptr[2];
      cptr[2] = tmp;
   }
   return (TRUE);
}

int get_bmp_body_bit_difference(bmp_file * original_bmp, bmp_file * new_bmp ){
   
   int i,j;
   int different_bits = 0;
   int pixel_index = 0;
   uint8_t mask = 0x01; //Mask that preserves lowest bit
   
   //Get the pointers to bmp bodys
   pixel * original_bmp_body = original_bmp->body;
   pixel * new_bmp_body = new_bmp->body;

   // printf("Hola 2\n");

   //Get body size
   int original_body_size = (sizeof(pixel) * (original_bmp->info_header->width) * (original_bmp->info_header->height));

   //We iterate through the different bits in original and new bmp bodys, and check if they differ or not
   for(i = 0;i < original_body_size ; i++){
      // printf("i = %d\n",i);
      for(j = 0; j < 8 ; j++){
            uint8_t original_bit = (original_bmp_body[pixel_index].colors[i%3] >> (7-j)) & mask;
            uint8_t new_bit = (new_bmp_body[pixel_index].colors[i%3] >> (7-j)) & mask;

            if(original_bit!=new_bit){
               different_bits++;
            }
      }
      if(i%3 == 0){
         pixel_index++;
      }        
   }

   return different_bits;

}

int get_bmp_body_byte_difference(bmp_file * original_bmp, bmp_file * new_bmp ){
   
   int i;
   int different_bytes = 0;
   int pixel_index = 0;
   
   //Get the pointers to bmp bodys
   pixel * original_bmp_body = original_bmp->body;
   pixel * new_bmp_body = new_bmp->body;

   //Get body size
   int original_body_size = (sizeof(pixel) * (original_bmp->info_header->width) * (original_bmp->info_header->height));

   //We iterate through the different bytes in original and new bmp bodys, and check if they differ or not
   for(i = 0;i < original_body_size ; i++){

      uint8_t original_byte = original_bmp_body[pixel_index].colors[i%3];
      uint8_t new_byte = new_bmp_body[pixel_index].colors[i%3];

      if(original_byte!=new_byte){
         different_bytes++;
      }

      if(i%3 == 0){
         pixel_index++;
      }        
   }

   return different_bytes;

}
