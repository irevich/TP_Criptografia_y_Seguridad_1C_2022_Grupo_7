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
        "   -v                                  Prints version and exits.\n"
        "   -embed                              Indicates that information will be hidden\n"
        "   -extract                            Indicates that information will be extracted\n"
        "   -in file                            File to be hidden through stenography\n"
        "   -p bitmapfile                       File to carry the hidden information\n"
        "   -out bitmapfile                     Bitmap file to be written with the hidden information\n"
        "   -steg <LSB1 | LSB4 | LSBI>          Stenography method to use\n"
        "   -a <aes128 | aes192 | aes256 | des> Encryption method to use\n"
        "   -m <ecb | cfb | ofb | cbc>          Encryption mode to use\n"
        "   -pass <password>                    Password to use\n"

        "\n",
        progname);
    exit(1);
}

void 
parse_args(const int argc, char **argv, struct parameters * parameters) {
    memset(parameters, 0, sizeof(*parameters));

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
            {"a", optional_argument, NULL, 'a'},
            {"m", optional_argument, NULL, 'm'},
            {"pass", optional_argument, NULL, 'k'},
            {0, 0, 0, 0}
        };
        const char * short_options = "hvexip:o:s:a:m:k:";
    while(opt = getopt_long(argc, argv, short_options, long_options, &option_index)){
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
                fprintf(stderr, "Invalid encryption algorithm\n");
                exit(1);
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
                fprintf(stderr, "Invalid encryption mode\n");
                exit(1);
            }
            break;
        case 'k':
            parameters->password = optarg;
            break;
        case '?':
            fprintf(stderr, "Invalid option\n");
            exit(1);
              
        default:
            usage(argv[0]);
            break;
        
        }
        

    }
}

static void print_parameters(struct parameters * parameters) {
    printf("Parameters:\n");
    printf("\tEmbed: %s\n", parameters->embed ? "true" : "false");
    printf("\tInput file: %s\n", parameters->input_file_path);
    printf("\tCarrier file: %s\n", parameters->carrier_file_path);
    printf("\tOutput file: %s\n", parameters->output_file_path);
    printf("\tStenography algorithm: %s\n", parameters->stego_algorithm == LSB1 ? "LSB1" : (parameters->stego_algorithm == LSB4 ? "LSB4" : "LSBI"));
    printf("\tEncryption algorithm: %s\n", parameters->encryption_algorithm == AES128 ? "AES128" : (parameters->encryption_algorithm == AES192 ? "AES192" : (parameters->encryption_algorithm == AES256 ? "AES256" : "DES")));
    printf("\tEncryption mode: %s\n", parameters->encryption_mode == ECB ? "ECB" : (parameters->encryption_mode == CFB ? "CFB" : (parameters->encryption_mode == OFB ? "OFB" : "CBC")));
    printf("\tPassword: %s\n", parameters->password);
}


int main(int argc, char **argv) {
    struct parameters parameters;
    parse_args(argc, argv, &parameters);
    print_parameters(&parameters);
}