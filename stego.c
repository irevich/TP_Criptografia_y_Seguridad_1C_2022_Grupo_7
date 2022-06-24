#include "include/stego.h"


// Hides bit in the least significant bit of the pixel
uint8_t hide_bit(uint8_t bit, uint8_t byte){
    if(bit){
        byte = byte | 0x01;     
    }
    else{
        byte = byte & 0xFE;
    }
    return byte;
}


uint32_t to_big_endian_32(uint32_t num){
    uint32_t b0,b1,b2,b3;
    uint32_t res;

    b0 = (num & 0x000000ff) << 24u;
    b1 = (num & 0x0000ff00) << 8u;
    b2 = (num & 0x00ff0000) >> 8u;
    b3 = (num & 0xff000000) >> 24u;

    res = b0 | b1 | b2 | b3;
    return res;
}

bmp_file * lsb1_embed(bmp_file * carrier_bmp, char * source_file_path){
    // Find size of source_file
    FILE * source_file = fopen(source_file_path, "r");
    fseek(source_file, 0, SEEK_END);
    uint32_t source_file_size = ftell(source_file);
    rewind(source_file);

    // Find extension of source_file
    char * token = strtok(source_file_path, ".");
    token = strtok(NULL, ".");
    char * dot = ".";
    char * extension = (char*) malloc(strlen(dot)+strlen(token)+1);
    strcpy(extension,dot);
    strcat(extension,token);

    //Allocate memory for output bmp
    bmp_file * output_bmp = (bmp_file *) malloc((sizeof(bmp_file)));
    output_bmp->header = carrier_bmp->header;
    output_bmp->info_header = carrier_bmp->info_header;
    output_bmp->body = (pixel *) malloc((sizeof(pixel) * (carrier_bmp->info_header->width) * (carrier_bmp->info_header->height)));
    // output_bmp->body = (uint8_t *) malloc((sizeof(uint8_t) * (carrier_bmp->info_header->width) * (carrier_bmp->info_header->height) * 3));

    // Check if source file can fit in the carrier bmp
    int needed_space = sizeof(source_file_size) + source_file_size + strlen(extension)+1;

    int carrier_bmp_body_size = carrier_bmp->info_header->width * carrier_bmp->info_header->height * 3;
    if( carrier_bmp_body_size < needed_space){
        fprintf(stderr, "Source file is too big to fit in the carrier bmp\n");
        exit(-1);
    }

    // Copy the carrier file body to the output file body
    memcpy(output_bmp->body,  carrier_bmp->body, carrier_bmp_body_size);

    // Create data to hide (size|bytes|extension)

    uint8_t * message = (uint8_t *) malloc(needed_space);

    // Size (Big endian)
    uint32_t source_file_size_big_endian = to_big_endian_32(source_file_size);
    memcpy(message, &source_file_size_big_endian, sizeof(source_file_size_big_endian));

    // File bytes
    uint8_t * file_bytes = (uint8_t *) malloc(source_file_size);
    if (fread(file_bytes, source_file_size, 1, source_file) != 1){
        fprintf(stderr, "Cannot read file bytes\n");
        exit(-1);
    }
    memcpy(message + sizeof(source_file_size), file_bytes, source_file_size);

    // Extension
    memcpy(message + sizeof(source_file_size) + source_file_size, extension, strlen(extension)+1);

    int bits_placed = 0;
    int pixel_index = 0;

    //Copy message to hide in output bmp
    for(int i = 0; i < needed_space; i++){
        for(int j = 0; j < 8 ; j++){
            uint8_t bit = (message[i] >> (7-j)) & 0x01; // Get the bit in the least significant bit
            uint8_t byte =  carrier_bmp->body[pixel_index].colors[bits_placed%3]; // Get the byte to hide the bit in
            // uint8_t byte =  carrier_bmp->body[i]; // Get the byte to hide the bit in
            uint8_t changed_byte = hide_bit(bit, byte); // Hide the bit in the byte
            output_bmp->body[pixel_index].colors[bits_placed%3] = changed_byte; // Update change in output_bmp
            // output_bmp->body[i] = changed_byte; // Update change in output_bmp

            bits_placed++;
            if(bits_placed%3 == 0){
                pixel_index++;
            }
        }        
    }
    
    return output_bmp;
}

FILE * lsb1_extract(bmp_file * carrier_bmp, char * output_file_name){

    int BLOCK = 500;
    uint8_t * message = (uint8_t *) malloc(sizeof(uint8_t) * BLOCK); 

    // Extract message from carrier bmp
    int message_index = 0;
    int bits_extracted = 0;
    uint8_t aux_byte = 0;
    for(int i = 0; i < carrier_bmp->info_header->width * carrier_bmp->info_header->height; i++){
        for(int j = 0; j < 3; j++){
            uint8_t byte = carrier_bmp->body[i].colors[j]; // Get the byte to extract the bit from
            // uint8_t byte = carrier_bmp->body[i]; // Get the byte to extract the bit from
            uint8_t bit = byte & 0x01; // Get the bit in the least significant bit
            aux_byte = aux_byte | (bit << (bits_extracted%8)); // Update the aux_byte
            bits_extracted++;

            if(bits_extracted%8 == 0){
                message_index = bits_extracted/8 - 1;
                if(message_index % BLOCK == 0){
                    message = realloc(message, sizeof(uint8_t) * (message_index + BLOCK)); // Reallocate memory for message
                }
                message[message_index] = aux_byte; // Update message
                aux_byte = 0;
            }
        }        
    }

    // Split message into source_file_size, source_file and extension
    // uint32_t file_size = ((uint32_t *) message)[0]; // First 4 bytes are file_size
    uint32_t file_size=0;
    for(int i=0;i<4;i++){
        file_size = (file_size|(*message)) <<8;
        message+=1;
    }
    
    uint8_t * data = (uint8_t *) malloc(sizeof(uint8_t) * file_size);

    // int allocated_memory = sizeof(*message);

    //Move the pointer to the data
    message = message + 4;
    for(int i = 0; i < file_size; i++){
        data[i] = *message;
        message += 1;   
    }
    char * extension = (char *) message; //e.g. ".txt"
    FILE * output_file = fopen(strcat(output_file_name, extension), "w");
    fwrite(data, sizeof(uint8_t), sizeof(file_size) + file_size + sizeof(extension), output_file);
    return output_file;
}


