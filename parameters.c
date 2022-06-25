#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <string.h>    /* memset */
#include <errno.h>
#include <getopt.h>

#include "include/parameters.h"


static void
authors(void) {
    fprintf(stderr, "ITBA Criptografía y Seguridad 2022 Primer Cuatrimestre, Equipo 7\n"
                    "Integrantes: Salustiano José Zavalía Pángaro, Igal Leonel Revich, Agustín Tormakh\n"
                    );
    exit(1);
}

static void
usage(const char *progname) {
    fprintf(stderr,
        "Usage: %s [OPTION]...\n"
        "\n"
        "   -h                                  Prints help and exits.\n"
        "   -A                                  Prints this project's magnificent authors.\n"
        "   -embed                              Indicates that information will be hidden\n"
        "   -extract                            Indicates that information will be extracted\n"
        "   -in file                            File to be hidden through stenography\n"
        "   -p bitmapfile                       File to carry the hidden information\n"
        "   -out bitmapfile                     Bitmap file to be written with the hidden information\n"
        "   -steg <LSB1 | LSB4 | LSBI>          Stenography method to use\n"
        "   -a <AES128 | AES192 | AES256 | DES> Encryption method to use\n"
        "   -m <ECB | CFB | OFB | CBC>          Encryption mode to use\n"
        "   --pass <password>                   Password to use\n"

        "\n",
        progname);
    exit(1);
}

void set_defaults(parameters_t * parameters){
    parameters->encryption_algorithm = -1;
    parameters->encryption_mode = -1;
    parameters->password = NULL;
}

parameters_t * parse_args(const int argc, char **argv) {
    parameters_t * parameters = (parameters_t *) malloc(sizeof(parameters_t));
    memset(parameters, 0, sizeof(*parameters));

    // Set default parameters
    set_defaults(parameters);

    // Configure parameter options
    char opt;
    int option_index = 0;
        static struct option long_options[] = {
            {"help", no_argument, NULL, 'h'},
            {"authors", no_argument, NULL, 'A'},
            {"embed", no_argument, NULL, 'e'},
            {"extract", no_argument, NULL, 'x'},
            {"in", required_argument, NULL, 'i'},
            {"p", required_argument, NULL, 'p'},
            {"out", required_argument, NULL, 'o'},
            {"steg", required_argument, NULL, 's'},
            {"a", required_argument, NULL, 'a'},
            {"m", required_argument, NULL, 'm'},
            {"pass", required_argument, NULL, 'k'},
            {0, 0, 0, 0}
        };
        const char * short_options = "hAexip:o:s:a:m:k:";

    // Read options
    while((opt = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1){
        switch (opt)
        {
        case 'h':
            usage(argv[0]);
            break;
        case 'A':
            authors();
            break;
        case 'e':
            parameters->embed = true;
            break;
        case 'x':
            parameters->embed = false;
            break;
        case 'i':
            parameters->input_file_path = optarg;
            break;
        case 'p':
            parameters->carrier_file_path = optarg;
            break;
        case 'o':
            parameters->output_file_path = optarg;
            break;
        case 's':
            if(strcmp(optarg, "LSB1") == 0) {
                parameters->stego_algorithm = LSB1;
            } else if(strcmp(optarg, "LSB4") == 0) {
                parameters->stego_algorithm = LSB4;
            } else if(strcmp(optarg, "LSBI") == 0) {
                parameters->stego_algorithm = LSBI;
            } else {
                fprintf(stderr, "Invalid steg algorithm\n");
                exit(1);
            }
            break;
        case 'a':
            if(strcmp(optarg, "AES128") == 0) {
                parameters->encryption_algorithm = AES128;
            } else if(strcmp(optarg, "AES192") == 0) {
                parameters->encryption_algorithm = AES192;
            } else if(strcmp(optarg, "AES256") == 0) {
                parameters->encryption_algorithm = AES256;
            } else if(strcmp(optarg, "DES") == 0) {
                parameters->encryption_algorithm = DES;
            } else {
                fprintf(stderr, "Invalid encryption algorithm, using default\n");
            }
            break;  
        case 'm':
            if(strcmp(optarg, "ECB") == 0) {
                parameters->encryption_mode = ECB;
            } else if(strcmp(optarg, "CFB") == 0) {
                parameters->encryption_mode = CFB;
            } else if(strcmp(optarg, "OFB") == 0) {
                parameters->encryption_mode = OFB;
            } else if(strcmp(optarg, "CBC") == 0) {
                parameters->encryption_mode = CBC;
            } else {
                fprintf(stderr, "Invalid encryption mode, using default\n");
            }
            break;
        case 'k':
            parameters->password = (uint8_t *)optarg;
            break;
        case '?':
            fprintf(stderr, "Invalid option\n");
            exit(1);
              
        default:
            //printf("En el default\n");
            //usage(argv[0]);
            break;
        
        }      
    }

    // Load defaults for encryption
    if(parameters->password != NULL){
        if(parameters->encryption_algorithm == -1)
            parameters->encryption_algorithm = AES128;
        
        if(parameters->encryption_mode == -1)
            parameters->encryption_mode = CBC;
    }
    return parameters;
}

// Remove comment for debugging tool
// static void print_parameters(struct parameters * parameters) {
//     printf("Parameters:\n");
//     printf("\tEmbed: %s\n", parameters->embed ? "true" : "false");
//     printf("\tInput file: %s\n", parameters->input_file_path);
//     printf("\tCarrier file: %s\n", parameters->carrier_file_path);
//     printf("\tOutput file: %s\n", parameters->output_file_path);
//     printf("\tStenography algorithm: %s\n", parameters->stego_algorithm == LSB1 ? "LSB1" : (parameters->stego_algorithm == LSB4 ? "LSB4" : "LSBI"));
//     printf("\tEncryption algorithm: %s\n", parameters->encryption_algorithm == AES128 ? "AES128" : (parameters->encryption_algorithm == AES192 ? "AES192" : (parameters->encryption_algorithm == AES256 ? "AES256" : "DES")));
//     printf("\tEncryption mode: %s\n", parameters->encryption_mode == ECB ? "ECB" : (parameters->encryption_mode == CFB ? "CFB" : (parameters->encryption_mode == OFB ? "OFB" : "CBC")));
//     printf("\tPassword: %s\n", parameters->password);
// }