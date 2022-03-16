#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "rsa.h"
#include "randstate.h"

#define OPTIONS "i:o:n:vh"
void usage(char *
        exec) { //function used in order to print out the usage options to the user on the command line
    fprintf(stderr,
        "SYNOPSIS\n"
        "	Decrypts data using RSA encryption.\n"
        "	Encrypted data is decrypted by the decrypt program.\n"
        "USAGE\n"
        "	%s [OPTIONS]\n"
        "OPTIONS \n"
        "-h		Display program help and usage.\n"
        "-v		Display verbose program output.\n"
        "-i infile	Input file of data to encrypt(default: stdin).\n"
        "-o ouftile	Output file for encrypted data(default: stdout).\n"
        "-n pbfile	Public key file(default: rsa.pub\n",
        exec);
}

int main(int argc, char **argv) {
    int opt = 0;
    FILE *infile = NULL;
    FILE *outfile = NULL;
    FILE *pvfile = NULL;
    char *priv_file = "rsa.priv";

    char *input = NULL;
    char *output = NULL;
    int verb = 0;
    //declaration of the variables that will be used based on what the user inputs in the command line
    mpz_t n, d;
    mpz_init(n);
    mpz_init(d);
    //initialization of all mpz_ts
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': input = optarg; break;
        case 'o': output = optarg; break;
        case 'n': priv_file = optarg; break;
        case 'v': verb = 1; break;
        case 'h': usage(argv[0]); break;
        } //switch statement that gives all the command line options
    }

    if (output != NULL
        && (fopen(output, "w"))
               == NULL) { //checks if an output file was specified and it doeesn't exist
        printf("Error opening %s.\n", output);
        exit(0);
    }

    if (input != NULL
        && (fopen(input, "r"))
               == NULL) { //checks if an intput file was specified and it doesn't exist
        printf("Error opening %s.\n", input);
        fclose(pvfile);
        fclose(infile);
        fclose(outfile);
        mpz_clear(n);
        mpz_clear(d);
        exit(0);
    }
    if (input != NULL
        && (fopen(input, "r")) != NULL) { //checks if an input file was specified and it does exis
        infile = fopen(input, "r");
    }

    if (fopen(priv_file, "r")
        == NULL) { //checks if a private key file was specified and it doesn't exist
        printf("Error opening %s.\n", priv_file);
        fclose(pvfile);
        fclose(infile);
        fclose(outfile);
        mpz_clear(n);
        mpz_clear(d);
        exit(0);
    }

    else if ((fopen(priv_file, "r"))
             != NULL) { //checks if a private key file was specified and it does exist
        pvfile = fopen(priv_file, "r");
        rsa_read_priv(n, d, pvfile);
    }
    if (output != NULL
        && (fopen(output, "w"))
               != NULL) { //checks if an output file was specified and it does exist
        outfile = fopen(output, "w");
    }
    if (output == NULL) { //checks if an output file wasn't specified
        outfile = stdout;
    }
    if (verb == 1) { //checks if verbose was turned on
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    if (input == NULL) { //checks if an input file wasn't specified
        infile = stdin;
    }
    rsa_decrypt_file(infile, outfile, n,
        d); //calls the decrypt_file function in order to decrypt the presented cipher text
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);
    mpz_clear(n);
    mpz_clear(d);
    //closes and clears all files and mpz_ts
}
