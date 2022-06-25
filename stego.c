#include "include/stego.h"
#include <string.h>

// Hides bits in the byte, uses a mask that preserves the desired bits
uint8_t hide_bits(uint8_t bits, uint8_t byte, uint8_t mask){
    byte = byte & (0xFF - mask); //Resets bits outside mask
    byte = byte | bits; //Sets bits to desired values
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

typedef struct{
    bmp_file * output_file;
    uint8_t * message;
    uint32_t needed_space;
} embedding_t;

void prepare_embedding(bmp_file * carrier_bmp, char * source_file_path, embedding_t * embedding){
    // Find size of source_file
    FILE * source_file = fopen(source_file_path, "r");
    if(source_file == NULL){
        fprintf(stderr, "Error opening source file in embed");
        exit(-1);
    }
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
    uint32_t needed_space = sizeof(source_file_size) + source_file_size + strlen(extension)+1;

    int carrier_bmp_body_size = carrier_bmp->info_header->width * carrier_bmp->info_header->height * 3;
    if( carrier_bmp_body_size < needed_space){
        fprintf(stderr, "Source file is too big to fit in the carrier bmp\n");
        exit(-1);
    }

    // Copy the carrier file body to the output file body
    if(memcpy(output_bmp->body,  carrier_bmp->body, carrier_bmp_body_size) == NULL){
        fprintf(stderr, "Failed copying body of carrier bmp into destination");
        exit(-1);
    }

    // Create data to hide (size|bytes|extension)
    uint8_t * message = (uint8_t *) malloc(needed_space);

    // Size (Big endian)
    uint32_t source_file_size_big_endian = to_big_endian_32(source_file_size);
    if(memcpy(message, &source_file_size_big_endian, sizeof(source_file_size_big_endian)) == NULL){
        fprintf(stderr, "Failed copying source file size into message");
        exit(-1);
    }

    // File bytes
    uint8_t * file_bytes = (uint8_t *) malloc(source_file_size);
    if (fread(file_bytes, source_file_size, 1, source_file) != 1){
        fprintf(stderr, "Cannot read file bytes\n");
        exit(-1);
    }

    if(memcpy(message + sizeof(source_file_size), file_bytes, source_file_size) == NULL){
        fprintf(stderr, "Failed copying source file content into message");
        exit(-1);
    }

    // Extension
    if(memcpy(message + sizeof(source_file_size) + source_file_size, extension, strlen(extension)+1) == NULL){
        fprintf(stderr, "Failed copying source file extension into message");
        exit(-1);
    }
    
    embedding->message = message;
    embedding->output_file = output_bmp;
    embedding->needed_space = needed_space;
}

bmp_file * lsb1_embed(bmp_file * carrier_bmp, char * source_file_path){
    embedding_t embedding;
    prepare_embedding(carrier_bmp, source_file_path, &embedding);

    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x01; //Mask that preserves lowest bit
    //Copy message to hide in output bmp
    for(int i = 0; i < embedding.needed_space; i++){
        for(int j = 0; j < 8 ; j++){
            uint8_t bit = (embedding.message[i] >> (7-j)) & mask; // Get the bit in the least significant bit
            uint8_t byte =  carrier_bmp->body[pixel_index].colors[bits_placed%3]; // Get the byte to hide the bit in
            // uint8_t byte =  carrier_bmp->body[i]; // Get the byte to hide the bit in
            uint8_t changed_byte = hide_bits(bit, byte, mask); // Hide the bit in the byte
            embedding.output_file->body[pixel_index].colors[bits_placed%3] = changed_byte; // Update change in output_bmp
            // output_bmp->body[i] = changed_byte; // Update change in output_bmp

            bits_placed++;
            if(bits_placed%3 == 0){
                pixel_index++;
            }
        }        
    }
    
    return embedding.output_file;
}

FILE * lsb1_extract(bmp_file * carrier_bmp, char * output_file_name){

    int i;
    
    int bits_placed = 0;
    int pixel_index = 0;

    //First, we get the pointer to the carrier_bmp body
    pixel * bmp_body = carrier_bmp->body;

    //Then, we create a uint32_t variable to save the file size
    uint32_t file_size = 0;

    //Then, we read the first 32 bytes of the carrier_bmp body to get the file size
    for(i = 0; i < 32 ;i++ ){
        // if(i==18){
        //     printf("Byte 18\n");
        // }
        // uint8_t current_byte = bmp_body[pixel_index].colors[bits_placed%3];
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & 0x01; // Get the least significant bit
        // printf("Current LSB in byte %d is %d\n",i,bit);
        file_size = file_size << 1; //Shift the bit to the left
        file_size = file_size | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    // file_size = to_big_endian_32(file_size);

    printf("The file size is %d\n",file_size);

    //Then, we have to read the file bytes

    //Number of bytes to read of the carrier bmp
    uint32_t number_of_carrier_bytes = file_size * 8;

    //Allocate memory for file bytes
    uint8_t * file_bytes =  (uint8_t *) malloc(file_size);

    //Read the corresponding bytes of the carrier bmp
    for(i = 0; i < number_of_carrier_bytes ;i++ ){
        // if(i==18){
        //     printf("Byte 18\n");
        // }
        // uint8_t current_byte = bmp_body[pixel_index].colors[bits_placed%3];
        
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & 0x01; // Get the least significant bit
       
        //Shift the bit to the left
        // printf("Current LSB in byte %d is %d\n",i,bit);
        file_bytes[i/8] = file_bytes[i/8] << 1; //Shift the bit to the left
        file_bytes[i/8] = file_bytes[i/8] | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        if(i!=0 && i%8==0){
            printf("The byte number %d is : %x\n",i/8,file_bytes[-1+i/8]);
        }

    }

    printf("The byte number %d is : %x\n",file_size,file_bytes[file_size -1]);
    //


    //Then, we read the extension
    // int BLOCK = 500;
    // uint8_t * extension = (uint8_t *) malloc(sizeof(uint8_t) * BLOCK);
    // int finish = 0;

    // int extension_bits = 0;

    // while(!finish){
        
    //     //Check if we have space allocated
    //     if((extension_bits/8) % BLOCK == 0){
    //         extension = realloc(extension, sizeof(uint8_t) * ((extension_bits/8) + BLOCK)); // Reallocate memory for extension
    //     }
        
    //     uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & 0x01; // Get the least significant bit
    //     //mal
    //     extension[extension_bits/8] = extension[extension_bits/8] << 1; //Shift the bit to the left
    //     extension[extension_bits/8] = extension[extension_bits/8] | bit; //Put the bit inside file_size

    //     bits_placed++;
    //     if(bits_placed%3 == 0){
    //         pixel_index++;
    //     }

    //     if(extension_bits!=0 && extension_bits%8==0){
    //         //Check if is \0 to see if we have finished
    //         if(strcmp((char*)extension[extension_bits/8],"\0")){
    //             finish = 1;
    //         }
    //     }
    //     extension_bits++;
    // }
    // printf("The extension is : %s\n",extension);
    //  extension = realloc(extension, sizeof(uint8_t) * (extension_bits/8)); // Final memory reallocation


    //Then, we create the new file
     char * extension = ".png";
    char * output_filepath = (char*) malloc(strlen(output_file_name)+strlen((char*)extension)+1);
    strcpy(output_filepath,output_file_name);
    // strcat(output_filepath,(char*)extension); //archivoOculto.png
    FILE * output_file = fopen(strcat(output_file_name, (char*) extension), "w");
    fwrite(file_bytes, sizeof(uint8_t), file_size, output_file);
    return output_file;


    //-----------VERSION VIEJA-------------------------------

    // int BLOCK = 500;
    // uint8_t * message = (uint8_t *) malloc(sizeof(uint8_t) * BLOCK); 

    // // Extract message from carrier bmp
    // int message_index = 0;
    // int bits_extracted = 0;
    // uint8_t aux_byte = 0;
    // for(int i = 0; i < carrier_bmp->info_header->width * carrier_bmp->info_header->height; i++){
    //     for(int j = 0; j < 3; j++){
    //         uint8_t byte = carrier_bmp->body[i].colors[j]; // Get the byte to extract the bit from
    //         // uint8_t byte = carrier_bmp->body[i]; // Get the byte to extract the bit from
    //         uint8_t bit = byte & 0x01; // Get the bit in the least significant bit
    //         aux_byte = aux_byte | (bit << (bits_extracted%8)); // Update the aux_byte
    //         bits_extracted++;

    //         if(bits_extracted%8 == 0){
    //             message_index = bits_extracted/8 - 1;
    //             if(message_index % BLOCK == 0){
    //                 message = realloc(message, sizeof(uint8_t) * (message_index + BLOCK)); // Reallocate memory for message
    //             }
    //             message[message_index] = aux_byte; // Update message
    //             aux_byte = 0;
    //         }
    //     }        
    // }

    // // Split message into source_file_size, source_file and extension
    // // uint32_t file_size = ((uint32_t *) message)[0]; // First 4 bytes are file_size
    // uint32_t file_size=0;
    // for(int i=0;i<4;i++){
    //     file_size = (file_size|(*message)) <<8;
    //     message+=1;
    // }
    
    // uint8_t * data = (uint8_t *) malloc(sizeof(uint8_t) * file_size);

    // // int allocated_memory = sizeof(*message);

    // //Move the pointer to the data
    // message = message + 4;
    // for(int i = 0; i < file_size; i++){
    //     data[i] = *message;
    //     message += 1;   
    // }
    // char * extension = (char *) message; //e.g. ".txt"
    // FILE * output_file = fopen(strcat(output_file_name, extension), "w");
    // fwrite(data, sizeof(uint8_t), sizeof(file_size) + file_size + sizeof(extension), output_file);
    // return output_file;
}

//---------------------------------------LSB4------------------------------------------
bmp_file * lsb4_embed(bmp_file * carrier_bmp, char * source_file_path){
    embedding_t embedding;
    prepare_embedding(carrier_bmp, source_file_path, &embedding);

    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x0F; //Mask that preserves the lower 4 bits
    //Copy message to hide in output bmp
    for(int i = 0; i < embedding.needed_space; i++){
        uint8_t aux_byte = embedding.message[i];
        for(int j = 0; j < 2 ; j++){
            uint8_t bits = (aux_byte >> (4-j*4)) & mask; // Get 4 bits, from left to right, into the lower 4 bits
            uint8_t byte =  carrier_bmp->body[pixel_index].colors[bits_placed%3]; // Get the byte to hide the bit in
            uint8_t changed_byte = hide_bits(bits, byte, mask); // Hide the 4 bits in the byte
            embedding.output_file->body[pixel_index].colors[bits_placed%3] = changed_byte; // Update change in output_bmp

            bits_placed++;
            if(bits_placed%3 == 0){
                pixel_index++;
            }
            aux_byte = embedding.message[i];
        }        
    }
    
    return embedding.output_file;
}

typedef struct{
    uint8_t pattern;
    uint32_t changed_count;
    uint32_t total_count;
    uint8_t swap;
} pattern_t;

//---------------------------------------LSBI------------------------------------------
bmp_file * lsbi_embed(bmp_file * carrier_bmp, char * source_file_path){
    embedding_t embedding;
    prepare_embedding(carrier_bmp, source_file_path, &embedding);

    // Preprare patterns
    pattern_t patterns[4];    
    for(uint8_t i = 0; i < 4; i++){
        patterns[i].pattern = i;
        patterns[i].changed_count = 0;
        patterns[i].total_count = 0;
        patterns[i].swap = FALSE;
    }

    int byte_counter = 0;
    int pixel_index = 0;
    uint8_t mask = 0x01; //Mask that preserves lowest bit
    //Copy message to hide in output bmp
    while(byte_counter < embedding.needed_space+4){
        //Skip 4 bytes to save space for pattern swaps back into the carrier        
        if( byte_counter >= 4){
            for(int j = 0; j < 8 ; j++){
                uint8_t bit = (embedding.message[byte_counter-4] >> (7-j)) & mask; // Get the bit in the least significant bit
                uint8_t byte =  carrier_bmp->body[pixel_index].colors[byte_counter%3]; // Get the byte to hide the bit in
                uint8_t pattern = (byte & 0x06) >> 1; //Extract 2nd and 3rd least significant bits for pattern
                
                patterns[pattern].total_count++;
                //Increase counter if least significant bit was changed
                if(bit != (byte & mask)){ 
                    patterns[pattern].changed_count++;
                }

                uint8_t changed_byte = hide_bits(bit, byte, mask); // Hide the bit in the byte
                embedding.output_file->body[pixel_index].colors[byte_counter%3] = changed_byte; // Update change in output_bmp
            }
        }
        byte_counter++;
        if(byte_counter%3 == 0){
            pixel_index++;
        }
    }

    write_bmp_file(embedding.output_file, "prueba.bmp");


    for(int i=0; i<4; i++){
        if(patterns[i].changed_count > patterns[i].total_count/2)
            patterns[i].swap = TRUE;
    }

    fprintf(stderr, "Pattern 00 - Changed: %u Total: %u Swap: %u\n", patterns[0].changed_count, patterns[0].total_count, patterns[0].swap);
    fprintf(stderr, "Pattern 01 - Changed: %u Total: %u Swap: %u\n", patterns[1].changed_count, patterns[1].total_count, patterns[1].swap);
    fprintf(stderr, "Pattern 10 - Changed: %u Total: %u Swap: %u\n", patterns[2].changed_count, patterns[2].total_count, patterns[2].swap);
    fprintf(stderr, "Pattern 11 - Changed: %u Total: %u Swap: %u\n", patterns[3].changed_count, patterns[3].total_count, patterns[3].swap);

    //Swap the hidden bit in bytes that require it in the output file
    byte_counter = 0;
    pixel_index = 0;
    while(byte_counter < embedding.needed_space + 4){
        //Place the code for which patterns need swapping
        if(byte_counter < 4){ 
            uint8_t byte = embedding.output_file->body[pixel_index].colors[byte_counter%3]; // Get the byte to hide the bit in
            uint8_t changed_byte = hide_bits(patterns[byte_counter].swap, byte, mask); // Hide the bit in the byte
            embedding.output_file->body[pixel_index].colors[byte_counter%3] = changed_byte;
        }
        else{
            uint8_t byte =  embedding.output_file->body[pixel_index].colors[byte_counter%3]; // Get the byte to hide the bit in
            uint8_t pattern = (byte & 0x06) >> 1;
        
            //Invert least bit if pattern requires swap
            if(patterns[pattern].swap){ 
                uint8_t changed_byte = hide_bits(!(byte & mask), byte, mask); // Invert the bit in the byte
                embedding.output_file->body[pixel_index].colors[byte_counter%3] = changed_byte; // Update change in output_bmp
            }          
        }
        byte_counter++;
        if(byte_counter%3 == 0){
            pixel_index++;
        }
    }

    return embedding.output_file;
}
