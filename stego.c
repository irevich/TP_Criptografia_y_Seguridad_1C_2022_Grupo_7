#include "include/stego.h"
#include "include/encription.h"

// Hides bits in the byte, uses a mask that preserves the desired bits
uint8_t hide_bits(uint8_t bits, uint8_t byte, uint8_t mask){
    byte = byte & (~mask); //Resets bits inside mask
    byte = byte | bits; //Sets bits to desired values, assuming mask is sufficient coverage
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

FILE * get_file_from_decryption(uint8_t * decrypted_text, char * output_file_name){
    
    int bytes_counter = 0;
    int i;

    //Reading the file size
    uint32_t file_size = 0;
    uint8_t current_byte = 0;
    for(i = 0; i < 4 ;i++ ){
        current_byte = decrypted_text[bytes_counter];
        file_size = file_size << 8;
        file_size = file_size | current_byte;
        bytes_counter++;
    }

    //Reading file bytes
    
    //Allocate memory for file bytes
    uint8_t * file_bytes = (uint8_t*) malloc(file_size);

    for(i = 0;i < file_size;i++){
        file_bytes[i] = decrypted_text[bytes_counter];
        bytes_counter++;
    }

    //Reading extension
    int BLOCK = 500;
    uint8_t * extension = (uint8_t *) malloc(sizeof(uint8_t) * BLOCK);
    int finish = 0;

    int extension_bytes = 0;

    while(!finish){
        
        //Check if we have space allocated
        if((extension_bytes) % BLOCK == 0){
            extension = realloc(extension, sizeof(uint8_t) * ((extension_bytes) + BLOCK)); // Reallocate memory for extension
        }

        extension[extension_bytes] = decrypted_text[bytes_counter];

        //Check if is \0 to see if we have finished
        if(extension[extension_bytes]==0){
            finish = 1;
        }

        extension_bytes++;
        bytes_counter++;
    }

    extension = realloc(extension, sizeof(uint8_t) * (extension_bytes)); // Final memory reallocation


    //Then, we create the new file
    char * output_filepath = (char*) malloc(strlen(output_file_name)+strlen((char*)extension)+1);
    strcpy(output_filepath,output_file_name);
    FILE * output_file = fopen(strcat(output_file_name, (char*) extension), "w");
    fwrite(file_bytes, sizeof(uint8_t), file_size, output_file);
    return output_file;
}

void prepare_embedding(bmp_file * carrier_bmp, char * source_file_path, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password, embedding_t * embedding){
    // Find size of source_file
    FILE * source_file = fopen(source_file_path, "r");
    if(source_file == NULL){
        fprintf(stderr, "Error opening source file in embed");
        exit(-1);
    }
    // Find extension of source_file
    char * token = strtok(source_file_path, ".");
    token = strtok(NULL, ".");
    char * dot = ".";
    char * extension = (char*) malloc(strlen(dot)+strlen(token)+1);
    strcpy(extension,dot);
    strcat(extension,token);
    int extension_size = strlen(extension) + 1;


    //Find source file size
    fseek(source_file, 0, SEEK_END);
    uint32_t source_file_size = ftell(source_file);
    rewind(source_file);    

    // Read file bytes
    uint8_t * file_bytes = (uint8_t *) malloc(source_file_size);
    if (fread(file_bytes, source_file_size, 1, source_file) != 1){
        fprintf(stderr, "Cannot read file bytes\n");
        exit(-1);
    }

    //Allocate memory for output bmp
    bmp_file * output_bmp = (bmp_file *) malloc((sizeof(bmp_file)));
    output_bmp->header = carrier_bmp->header;
    output_bmp->info_header = carrier_bmp->info_header;
    int carrier_bmp_body_size = (sizeof(pixel) * (carrier_bmp->info_header->width) * (carrier_bmp->info_header->height));
    output_bmp->body = (pixel *) malloc(carrier_bmp_body_size);

    // Copy the carrier file body to the output file body
    if(memcpy(output_bmp->body,  carrier_bmp->body, carrier_bmp_body_size) == NULL){
        fprintf(stderr, "Failed copying body of carrier bmp into destination");
        exit(-1);
    }

    // Message is to be encrypted
    if(password != NULL){

        // Allocate size for file bytes and extension into a buffer
        int file_bytes_and_extension_size = source_file_size + extension_size;
        uint8_t * file_bytes_and_extension = malloc(file_bytes_and_extension_size);

        // Copy file bytes onto buffer
        if(memcpy(file_bytes_and_extension , file_bytes, source_file_size) == NULL){
            fprintf(stderr, "Failed copying source file content into buffer for encryption");
            exit(-1);
        }

        // Copy extension onto buffer
        if(memcpy(file_bytes_and_extension + source_file_size, extension, extension_size) == NULL){
            fprintf(stderr, "Failed copying source file extension into buffer for encryption");
            exit(-1);
        }

        // Encrypt file bytes and extension in their buffer
        uint8_t * cyphertext = malloc(file_bytes_and_extension_size + EVP_MAX_BLOCK_LENGTH);
        uint32_t cyphertext_size;
        cyphertext_size = encrypt(encryption_algorithm, encryption_mode, file_bytes_and_extension, file_bytes_and_extension_size, password, cyphertext);
        cyphertext = realloc(cyphertext, cyphertext_size); // Trim buffer size to match the size actually used by encryption

        //Encrypt new size
        cyphertext_size = to_big_endian_32(cyphertext_size);
        uint8_t * encrypted_cyphertext_size = malloc(sizeof(cyphertext_size) + EVP_MAX_BLOCK_LENGTH)  ;
        uint32_t encrypted_cyphertext_size_size;
        encrypted_cyphertext_size_size = encrypt(encryption_algorithm, encryption_mode, (uint8_t *) &cyphertext_size, sizeof(cyphertext_size), password, encrypted_cyphertext_size);
        encrypted_cyphertext_size = realloc(encrypted_cyphertext_size, encrypted_cyphertext_size_size); // Trim buffer size to match the size actually used by encryption

        // Check if the size of the encrypted cyphertext size fits in the allotted 4 bytes
        if(encrypted_cyphertext_size_size != sizeof(cyphertext_size)){
            fprintf(stderr, "Encrypted message size doesn't fit in allotted 4 bytes, something went wrong in the encryption for the embedding");
            exit(-1);
        }

        uint32_t needed_space = sizeof(cyphertext_size) + cyphertext_size;
        uint8_t * encrypted_message = malloc(needed_space);
        
        // Copy encrypted message size onto encrypted message
        if(memcpy(encrypted_message, encrypted_cyphertext_size, sizeof(cyphertext_size)) == NULL){
            fprintf(stderr, "Error copying encrypted file size onto message");
            exit(-1);
        }

        // Copy encrypted file bytes and extension onto encrypted message
        if(memcpy(encrypted_message + sizeof(cyphertext_size), cyphertext, cyphertext_size) == NULL){
            fprintf(stderr, "Error copying encrypted file and extension onto message");
            exit(-1);
        }

        embedding->message = encrypted_message;
        embedding->output_file = output_bmp;
        embedding->needed_space = needed_space;
        return;
    }

    // Check if source file can fit in the carrier bmp
    uint32_t needed_space = sizeof(source_file_size) + source_file_size + strlen(extension)+1;

    if( carrier_bmp_body_size < needed_space){
        fprintf(stderr, "Source file is too big to fit in the carrier bmp\n");
        exit(-1);
    }    

    // Create data to hide (size|bytes|extension)
    uint8_t * message = (uint8_t *) malloc(needed_space);

    // Copy size (Big endian) into message
    uint32_t source_file_size_big_endian = to_big_endian_32(source_file_size);
    if(memcpy(message, &source_file_size_big_endian, sizeof(source_file_size_big_endian)) == NULL){
        fprintf(stderr, "Failed copying source file size into message");
        exit(-1);
    }

    // Copy file bytes onto message
    if(memcpy(message + sizeof(source_file_size), file_bytes, source_file_size) == NULL){
        fprintf(stderr, "Failed copying source file content into message");
        exit(-1);
    }

    // Copy extension onto message
    if(memcpy(message + sizeof(source_file_size) + source_file_size, extension, strlen(extension)+1) == NULL){
        fprintf(stderr, "Failed copying source file extension into message");
        exit(-1);
    }    
    
    embedding->message = message;
    embedding->output_file = output_bmp;
    embedding->needed_space = needed_space;
}

//---------------------------------------LSB1------------------------------------------
bmp_file * lsb1_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){
    embedding_t embedding;
    prepare_embedding(carrier_bmp, source_file_path, encryption_algorithm, encryption_mode, password, &embedding);

    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x01; //Mask that preserves lowest bit
    //Copy message to hide in output bmp
    for(int i = 0; i < embedding.needed_space; i++){
        for(int j = 0; j < 8 ; j++){
            uint8_t bit = (embedding.message[i] >> (7-j)) & mask; // Get the bit in the least significant bit
            uint8_t byte =  carrier_bmp->body[pixel_index].colors[bits_placed%3]; // Get the byte to hide the bit in

            uint8_t changed_byte = hide_bits(bit, byte, mask); // Hide the bit in the byte
            embedding.output_file->body[pixel_index].colors[bits_placed%3] = changed_byte; // Update change in output_bmp

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
    uint8_t mask = 0x01; //Mask that preserves lowest bit

    //First, we get the pointer to the carrier_bmp body
    pixel * bmp_body = carrier_bmp->body;

    //Then, we create a uint32_t variable to save the file size
    uint32_t file_size = 0;

    //Then, we read the first 32 bytes of the carrier_bmp body to get the file size
    for(i = 0; i < 32 ;i++ ){
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the least significant bit
        // printf("Current LSB in byte %d is %d\n",i,bit);
        file_size = file_size << 1; //Shift the bit to the left
        file_size = file_size | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    // printf("The file size is %d\n",file_size);

    //Then, we have to read the file bytes

    //Number of bytes to read of the carrier bmp
    uint32_t number_of_carrier_bytes = file_size * 8;

    //Allocate memory for file bytes
    uint8_t * file_bytes =  (uint8_t *) malloc(file_size);

    //Read the corresponding bytes of the carrier bmp
    for(i = 0; i < number_of_carrier_bytes ;i++ ){
        
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the least significant bit
       
        //Shift the bit to the left
        // printf("Current LSB in byte %d is %d\n",i,bit);
        file_bytes[i/8] = file_bytes[i/8] << 1; //Shift the bit to the left
        file_bytes[i/8] = file_bytes[i/8] | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        // if(i!=0 && i%8==0){
        //     printf("The byte number %d is : %x\n",i/8,file_bytes[-1+i/8]);
        // }

    }

    // printf("The byte number %d is : %x\n",file_size,file_bytes[file_size -1]);

    //Then, we read the extension
    int BLOCK = 500;
    uint8_t * extension = (uint8_t *) malloc(sizeof(uint8_t) * BLOCK);
    int finish = 0;

    int extension_bits = 0;

    while(!finish){
        
        //Check if we have space allocated
        if((extension_bits/8) % BLOCK == 0){
            extension = realloc(extension, sizeof(uint8_t) * ((extension_bits/8) + BLOCK)); // Reallocate memory for extension
        }
        
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the least significant bit

        extension[extension_bits/8] = extension[extension_bits/8] << 1; //Shift the bit to the left
        extension[extension_bits/8] = extension[extension_bits/8] | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        if(extension_bits!=0 && extension_bits%8==0){
            //Check if is \0 to see if we have finished
            if(extension[extension_bits/8]==0){
                finish = 1;
            }
        }
        extension_bits++;
    }

    extension = realloc(extension, sizeof(uint8_t) * (extension_bits/8)); // Final memory reallocation


    //Then, we create the new file
    char * output_filepath = (char*) malloc(strlen(output_file_name)+strlen((char*)extension)+1);
    strcpy(output_filepath,output_file_name);
    FILE * output_file = fopen(strcat(output_file_name, (char*) extension), "w");
    fwrite(file_bytes, sizeof(uint8_t), file_size, output_file);
    return output_file;

}

FILE * lsb1_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){
    
    //DECRYPTION
    int PADDING = 500;
    
    int i;
    
    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x01; //Mask that preserves lowest bit

    //First, we get the pointer to the carrier_bmp body
    pixel * bmp_body = carrier_bmp->body;

    //Then, we create a uint32_t variable to save the encrypted file size
    uint32_t encrypted_file_size = 0;

    //Then, we read the first 32 bytes of the carrier_bmp body to get the file size
    for(i = 0; i < 32 ;i++ ){
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the least significant bit
        // printf("Current LSB in byte %d is %d\n",i,bit);
        encrypted_file_size = encrypted_file_size << 1; //Shift the bit to the left
        encrypted_file_size = encrypted_file_size | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    // printf("The file size is %d\n",file_size);

    //Then, we have to read the file bytes

    //Number of bytes to read of the carrier bmp
    uint32_t number_of_carrier_bytes = encrypted_file_size * 8;

    //Allocate memory for encrypted file bytes
    uint8_t * encrypted_file_bytes =  (uint8_t *) malloc(encrypted_file_size);

    //Read the corresponding bytes of the carrier bmp
    for(i = 0; i < number_of_carrier_bytes ;i++ ){
        
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the least significant bit
       
        //Shift the bit to the left
        // printf("Current LSB in byte %d is %d\n",i,bit);
        encrypted_file_bytes[i/8] = encrypted_file_bytes[i/8] << 1; //Shift the bit to the left
        encrypted_file_bytes[i/8] = encrypted_file_bytes[i/8] | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        // if(i!=0 && i%8==0){
        //     printf("The byte number %d is : %x\n",i/8,file_bytes[-1+i/8]);
        // }

    }

    // Then, we have to decrypt the encrypted text

    //Allocate memory for the decrypted text
    uint8_t * decrypted_text = (uint8_t *) malloc(encrypted_file_size+PADDING);

     // Decrypt the ciphertext
    int decrypted_text_len = decrypt(encryption_algorithm,encryption_mode,encrypted_file_bytes, encrypted_file_size,(unsigned char *)password,decrypted_text);

    //Reallocate the memory for the decrypted text
    decrypted_text = (uint8_t *) realloc(decrypted_text,decrypted_text_len+1);

    // Add a NULL terminator. We are expecting printable text
    decrypted_text[decrypted_text_len] = '\0';

    //EXTRACTING
    return get_file_from_decryption(decrypted_text,output_file_name);

}



//---------------------------------------LSB4------------------------------------------
bmp_file * lsb4_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){
    embedding_t embedding;
    prepare_embedding(carrier_bmp, source_file_path, encryption_algorithm, encryption_mode, password, &embedding);

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

FILE * lsb4_extract(bmp_file * carrier_bmp, char * output_file_name){

    int i;
    
    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x0F; //Mask that preserves the lower 4 bits

    //First, we get the pointer to the carrier_bmp body
    pixel * bmp_body = carrier_bmp->body;

    //Then, we create a uint32_t variable to save the file size
    uint32_t file_size = 0;

    //Then, we read the first 8 bytes of the carrier_bmp body to get the file size
    for(i = 0; i < 8 ;i++ ){
        uint8_t bits = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the 4 least significant bits
        file_size = file_size << 4; //Shift the bits to the left
        file_size = file_size | bits; //Put the bits inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    // printf("The file size is %d\n",file_size);

    //Then, we have to read the file bytes

    //Number of bytes to read of the carrier bmp
    uint32_t number_of_carrier_bytes = file_size * 2;

    //Allocate memory for file bytes
    uint8_t * file_bytes =  (uint8_t *) malloc(file_size);

    //Read the corresponding bytes of the carrier bmp
    for(i = 0; i < number_of_carrier_bytes ;i++ ){
        
        uint8_t bits = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the 4 least significant bits
       
        //Shift the bit to the left
        // printf("Current LSB in byte %d is %d\n",i,bit);
        file_bytes[i/2] = file_bytes[i/2] << 4; //Shift the bits to the left
        file_bytes[i/2] = file_bytes[i/2] | bits; //Put the bits inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        // if(i!=0 && i%8==0){
        //     printf("The byte number %d is : %x\n",i/8,file_bytes[-1+i/8]);
        // }

    }

    // printf("The byte number %d is : %x\n",file_size,file_bytes[file_size -1]);

    //Then, we read the extension
    int BLOCK = 500;
    uint8_t * extension = (uint8_t *) malloc(sizeof(uint8_t) * BLOCK);
    int finish = 0;

    int extension_bits = 0;

    while(!finish){
        
        //Check if we have space allocated
        if((extension_bits/2) % BLOCK == 0){
            extension = realloc(extension, sizeof(uint8_t) * ((extension_bits/2) + BLOCK)); // Reallocate memory for extension
        }
        
        uint8_t bits = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the 4 least significant bits

        extension[extension_bits/2] = extension[extension_bits/2] << 4; //Shift the bits to the left
        extension[extension_bits/2] = extension[extension_bits/2] | bits; //Put the bits inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        if(extension_bits!=0 && extension_bits%2==0){
            //Check if is \0 to see if we have finished
            if(extension[extension_bits/2]==0){
                finish = 1;
            }
        }
        extension_bits++;
    }

    extension = realloc(extension, sizeof(uint8_t) * (extension_bits/2)); // Final memory reallocation


    //Then, we create the new file
    char * output_filepath = (char*) malloc(strlen(output_file_name)+strlen((char*)extension)+1);
    strcpy(output_filepath,output_file_name);
    FILE * output_file = fopen(strcat(output_file_name, (char*) extension), "w");
    fwrite(file_bytes, sizeof(uint8_t), file_size, output_file);
    return output_file;

}


FILE * lsb4_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){
    
    //DECRYPTION
    int PADDING = 500;
    
    int i;
    
    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x0F; //Mask that preserves the lower 4 bits

    //First, we get the pointer to the carrier_bmp body
    pixel * bmp_body = carrier_bmp->body;

    //Then, we create a uint32_t variable to save the encrypted file size
    uint32_t encrypted_file_size = 0;

    //Then, we read the first 32 bytes of the carrier_bmp body to get the file size
    for(i = 0; i < 8 ;i++ ){
        uint8_t bits = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the 4 least significant bits
        encrypted_file_size = encrypted_file_size << 4; //Shift the bits to the left
        encrypted_file_size = encrypted_file_size | bits; //Put the bits inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    // printf("The file size is %d\n",file_size);

    //Then, we have to read the file bytes

    //Number of bytes to read of the carrier bmp
    uint32_t number_of_carrier_bytes = encrypted_file_size * 2;

    //Allocate memory for encrypted file bytes
    uint8_t * encrypted_file_bytes =  (uint8_t *) malloc(encrypted_file_size);

    //Read the corresponding bytes of the carrier bmp
    for(i = 0; i < number_of_carrier_bytes ;i++ ){
        
        uint8_t bits = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the 4 least significant bits
       
        //Shift the bit to the left
        // printf("Current LSB in byte %d is %d\n",i,bit);
        encrypted_file_bytes[i/2] = encrypted_file_bytes[i/2] << 4; //Shift the bits to the left
        encrypted_file_bytes[i/2] = encrypted_file_bytes[i/2] | bits; //Put the bits inside encrypted_file_bytes

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        // if(i!=0 && i%8==0){
        //     printf("The byte number %d is : %x\n",i/8,file_bytes[-1+i/8]);
        // }

    }

    // Then, we have to decrypt the encrypted text

    //Allocate memory for the decrypted text
    uint8_t * decrypted_text = (uint8_t *) malloc(encrypted_file_size+PADDING);

     // Decrypt the ciphertext
    int decrypted_text_len = decrypt(encryption_algorithm,encryption_mode,encrypted_file_bytes, encrypted_file_size,(unsigned char *)password,decrypted_text);

    //Reallocate the memory for the decrypted text
    decrypted_text = (uint8_t *) realloc(decrypted_text,decrypted_text_len+1);

    // Add a NULL terminator. We are expecting printable text
    decrypted_text[decrypted_text_len] = '\0';

    //EXTRACTING
    return get_file_from_decryption(decrypted_text,output_file_name);

}

//---------------------------------------LSBI------------------------------------------

typedef struct{
    uint8_t pattern;
    uint32_t changed_count;
    uint32_t total_count;
    uint8_t swap;
} pattern_t;


bmp_file * lsbi_embed_with_encryption(bmp_file * carrier_bmp, char * source_file_path, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){
    embedding_t embedding;
    prepare_embedding(carrier_bmp, source_file_path, encryption_algorithm, encryption_mode, password, &embedding);

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
    for( int i= 0; i < embedding.needed_space+4; i++){
        //Skip 4 bytes to save space for pattern swaps back into the carrier    
        if(byte_counter < 4){
            byte_counter++;
            if(byte_counter%3 == 0){
                pixel_index++;
            }
        }    
        else{
            uint8_t message_byte = embedding.message[i-4];
            for(int j = 0; j < 8 ; j++){
                uint8_t bit = (message_byte >> (7-j)) & mask; // Get the bit in the least significant bit
                uint8_t byte =  carrier_bmp->body[pixel_index].colors[byte_counter%3]; // Get the byte to hide the bit in
                uint8_t pattern = (byte & 0x06) >> 1; //Extract 2nd and 3rd least significant bits for pattern
                
                patterns[pattern].total_count++;
                //Increase counter if least significant bit was swapped
                if(bit == ((~byte) & mask)){
                    patterns[pattern].changed_count++;
                }

                byte_counter++;
                if(byte_counter%3 == 0){
                    pixel_index++;
                }
            }
        }
    } 

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
    for(int i = 0; i < embedding.needed_space + 4; i++){
        //Place the code for which patterns need swapping
        if(byte_counter < 4){ 
            uint8_t byte = carrier_bmp->body[pixel_index].colors[byte_counter%3]; // Get the byte to hide the bit in
            uint8_t changed_byte = hide_bits(patterns[byte_counter].swap, byte, mask); // Hide the bit in the byte
            embedding.output_file->body[pixel_index].colors[byte_counter%3] = changed_byte;
            
            byte_counter++;
            if(byte_counter%3 == 0){
                pixel_index++;
            }
        }
        else{
            for(int j = 0; j < 8 ; j++){
                uint8_t bit = (embedding.message[i-4] >> (7-j)) & mask; // Get the bit in the least significant bit
                uint8_t byte =  carrier_bmp->body[pixel_index].colors[byte_counter%3]; // Get the byte to hide the bit in
                uint8_t pattern = (byte & 0x06) >> 1; //Extract 2nd and 3rd least significant bits for pattern
                
                //Invert least bit if pattern requires swap
                if(patterns[pattern].swap){ 
                    bit = (~bit) & mask;
                }

                uint8_t changed_byte = hide_bits(bit, byte, mask); // Hide the bit in the byte
                embedding.output_file->body[pixel_index].colors[byte_counter%3] = changed_byte; // Update change in output_bmp
                
                byte_counter++;
                if(byte_counter%3 == 0){
                    pixel_index++;
                }
            }   
        }        
    }


    return embedding.output_file;
}


FILE * lsbi_extract(bmp_file * carrier_bmp, char * output_file_name){

    int i;
    
    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x01; //Mask that preserves lowest bit

    //First, we get the pointer to the carrier_bmp body
    pixel * bmp_body = carrier_bmp->body;

    //Initialize structure to save pattern info
    pattern_t patterns[4];

    //Read first 4 bytes to get pattern info
    for(int i = 0; i < 4; i++){
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the least significant bit
        patterns[i].swap = bit;

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    uint32_t file_size = 0;

    //Then, we read the first 32 bytes of the carrier_bmp body to get the file size
    for(i = 0; i < 32 ;i++ ){
        uint8_t byte = (bmp_body[pixel_index].colors[bits_placed%3]);
        uint8_t bit = byte & mask; // Get the least significant bit
        uint8_t pattern = (byte & 0x06) >> 1;

        if(patterns[pattern].swap){
            bit = (~bit) & mask; //Invert the bit if needed
        }
        
        file_size = file_size << 1; //Shift the bit to the left
        file_size = file_size | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    fprintf(stderr, "Payload size: %d\n", file_size);

    //Then, we have to read the file bytes
    //Number of bytes to read of the carrier bmp
    uint32_t number_of_carrier_bytes = file_size * 8;

    //Allocate memory for file bytes
    uint8_t * file_bytes =  (uint8_t *) malloc(file_size);

    //Read the corresponding bytes of the carrier bmp
    for(i = 0; i < number_of_carrier_bytes ;i++ ){
        uint8_t byte = (bmp_body[pixel_index].colors[bits_placed%3]);
        uint8_t bit = byte & mask; // Get the least significant bit
        uint8_t pattern = (byte & 0x06) >> 1;

        if(patterns[pattern].swap){
            bit = (~bit) & mask; //Invert the bit if needed
        }
       
        file_bytes[i/8] = file_bytes[i/8] << 1; //Shift the bit to the left
        file_bytes[i/8] = file_bytes[i/8] | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }
    }

    //Then, we read the extension
    int BLOCK = 500;
    uint8_t * extension = (uint8_t *) malloc(sizeof(uint8_t) * BLOCK);
    int finish = 0;

    int extension_bits = 0;

    while(!finish){
        
        //Check if we have space allocated
        if((extension_bits/8) % BLOCK == 0){
            extension = realloc(extension, sizeof(uint8_t) * ((extension_bits/8) + BLOCK)); // Reallocate memory for extension
        }
        
        uint8_t byte = (bmp_body[pixel_index].colors[bits_placed%3]);
        uint8_t bit = byte & mask; // Get the least significant bit
        uint8_t pattern = (byte & 0x06) >> 1;

        if(patterns[pattern].swap){
            bit = (~bit) & mask; //Invert the bit if needed
        }

        extension[extension_bits/8] = extension[extension_bits/8] << 1; //Shift the bit to the left
        extension[extension_bits/8] = extension[extension_bits/8] | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        if(extension_bits!=0 && extension_bits%8==0){
            //Check if is \0 to see if we have finished
            if(extension[extension_bits/8]==0){
                finish = 1;
            }
        }
        extension_bits++;
    }

    extension = realloc(extension, sizeof(uint8_t) * (extension_bits/8)); // Final memory reallocation

    //Then, we create the new file
    char * output_filepath = (char*) malloc(strlen(output_file_name)+strlen((char*)extension)+1);
    strcpy(output_filepath,output_file_name);
    FILE * output_file = fopen(strcat(output_file_name, (char*) extension), "w");
    fwrite(file_bytes, sizeof(uint8_t), file_size, output_file);
    return output_file;

}

FILE * lsbi_extract_with_encryption(bmp_file * carrier_bmp, char * output_file_name, encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){

    //DECRYPTION
    int PADDING = 500;

    int i;
    
    int bits_placed = 0;
    int pixel_index = 0;
    uint8_t mask = 0x01; //Mask that preserves lowest bit

    //First, we get the pointer to the carrier_bmp body
    pixel * bmp_body = carrier_bmp->body;

    //Initialize structure to save pattern info
    pattern_t patterns[4];

    //Read first 4 bytes to get pattern info
    for(int i = 0; i < 4; i++){
        uint8_t bit = (bmp_body[pixel_index].colors[bits_placed%3]) & mask; // Get the least significant bit
        patterns[i].swap = bit;

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    fprintf(stderr, "Pattern 00 - Swap: %u\n", patterns[0].swap);
    fprintf(stderr, "Pattern 01 - Swap: %u\n", patterns[1].swap);
    fprintf(stderr, "Pattern 10 - Swap: %u\n", patterns[2].swap);
    fprintf(stderr, "Pattern 11 - Swap: %u\n", patterns[3].swap);


    //Then, we create a uint32_t variable to save the encrypted file size
    uint32_t encrypted_file_size = 0;

    //Then, we read the first 32 bytes of the carrier_bmp body to get the file size
    for(i = 0; i < 32 ;i++ ){
        uint8_t byte = (bmp_body[pixel_index].colors[bits_placed%3]);
        uint8_t bit = byte & mask; // Get the least significant bit
        uint8_t pattern = (byte & 0x06) >> 1;

        if(patterns[pattern].swap){
            bit = (~bit) & mask; //Invert the bit if needed
        }
        
        encrypted_file_size = encrypted_file_size << 1; //Shift the bit to the left
        encrypted_file_size = encrypted_file_size | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        } 
    }

    // printf("The file size is %d\n",file_size);

    //Then, we have to read the file bytes

    //Number of bytes to read of the carrier bmp
    uint32_t number_of_carrier_bytes = encrypted_file_size * 8;

    //Allocate memory for encrypted file bytes
    uint8_t * encrypted_file_bytes =  (uint8_t *) malloc(encrypted_file_size);

    //Read the corresponding bytes of the carrier bmp
    for(i = 0; i < number_of_carrier_bytes ;i++ ){
        uint8_t byte = (bmp_body[pixel_index].colors[bits_placed%3]);
        uint8_t bit = byte & mask; // Get the least significant bit
        uint8_t pattern = (byte & 0x06) >> 1;

        if(patterns[pattern].swap){
            bit = (~bit) & mask; //Invert the bit if needed
        }
       
        //Shift the bit to the left
        // printf("Current LSB in byte %d is %d\n",i,bit);
        encrypted_file_bytes[i/8] = encrypted_file_bytes[i/8] << 1; //Shift the bit to the left
        encrypted_file_bytes[i/8] = encrypted_file_bytes[i/8] | bit; //Put the bit inside file_size

        bits_placed++;
        if(bits_placed%3 == 0){
            pixel_index++;
        }

        // if(i!=0 && i%8==0){
        //     printf("The byte number %d is : %x\n",i/8,file_bytes[-1+i/8]);
        // }

    }

    // printf("The byte number %d is : %x\n",file_size,file_bytes[file_size -1]);


    // Then, we have to decrypt the encrypted text

    //Allocate memory for the decrypted text
    uint8_t * decrypted_text = (uint8_t *) malloc(encrypted_file_size+PADDING);

     // Decrypt the ciphertext
    int decrypted_text_len = decrypt(encryption_algorithm,encryption_mode,encrypted_file_bytes, encrypted_file_size,(unsigned char *)password,decrypted_text);

    //Reallocate the memory for the decrypted text
    decrypted_text = (uint8_t *) realloc(decrypted_text,decrypted_text_len+1);

    // Add a NULL terminator. We are expecting printable text
    decrypted_text[decrypted_text_len] = '\0';

    //EXTRACTING
    return get_file_from_decryption(decrypted_text,output_file_name);

}

bmp_file * (*embed_with_encryption_functions[3])(bmp_file * carrier_bmp, char * input_file_path,encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password) = {lsb1_embed_with_encryption,lsb4_embed_with_encryption,lsbi_embed_with_encryption};

FILE * (*extract_functions[3])(bmp_file * carrier_bmp, char * output_file_name)= {lsb1_extract,lsb4_extract,lsbi_extract};

FILE * (*extract_with_encryption_functions[3])(bmp_file * carrier_bmp, char * output_file_name,encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password) = {lsb1_extract_with_encryption,lsb4_extract_with_encryption,lsbi_extract_with_encryption};

bmp_file * embed(stego_algorithm_t stego,bmp_file * carrier_bmp, char * input_file_path,encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){
    return (*embed_with_encryption_functions[stego])(carrier_bmp,input_file_path,encryption_algorithm,encryption_mode,password);   
}

FILE * extract(stego_algorithm_t stego,bmp_file * carrier_bmp, char * output_file_name,encryption_algorithm_t encryption_algorithm, encryption_mode_t encryption_mode, uint8_t * password){
    if(password==NULL){
        return (*extract_functions[stego])(carrier_bmp,output_file_name);
    }
    return (*extract_with_encryption_functions[stego])(carrier_bmp,output_file_name,encryption_algorithm, encryption_mode, password);
}

