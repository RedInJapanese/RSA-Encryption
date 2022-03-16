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
void usage(
    char *exec) { //usage function used to print the options the user can use on the command line
    fprintf(stderr,
        "SYNOPSIS\n"
        "	Encrypts data using RSA encryption.\n"
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
    FILE *pbfile = NULL;
    char *pub_file = "rsa.pub";

    char *input = NULL;
    char *output = NULL;
    int verb = 0;
    //declaration of variables used based on the user's input on the command line
    mpz_t n, e, s, user, m;
    mpz_init(n);
    mpz_init(e);
    mpz_init(s);
    mpz_init(user);
    mpz_init(m);
    //initialization of all mpz_ts being used
    char *test = (char *) malloc(128 * sizeof(char));
    //initialization of string being used to get the username(128 bytes are allocated because that's the limit of a username on unix)

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': input = optarg; break;
        case 'o': output = optarg; break;
        case 'n': pub_file = optarg; break;
        case 'v': verb = 1; break;
        case 'h': usage(argv[0]); break;
        } //switch statement of all the command line options
    }
    if (output != NULL
        && (fopen(output, "w"))
               == NULL) { //if statement to check if the user specified a file but the file doesn't exist
        printf("Error opening %s.\n", output);
        fclose(outfile);
        exit(0);
    }

    if (input != NULL
        && (fopen(input, "r"))
               == NULL) { //checks if the user specified an input file but the file doesn't exist
        printf("Error opening %s.\n", input);
        fclose(infile);
        exit(0);
    }
    if (input != NULL
        && (fopen(input, "r"))
               != NULL) { //checks if the user specified an input file and the file does exist
        infile = fopen(input, "r");
    }

    if ((fopen(pub_file, "r"))
        == NULL) { //checks if the user specified a public key file and the file doesn't exist
        printf("Error opening %s.\n", pub_file);
        fclose(pbfile);
        exit(0);
    }

    else if (fopen(pub_file, "r")
             != NULL) { //checks if the user specified a public key file and file does exist
        pbfile = fopen(pub_file, "r");
        rsa_read_pub(n, e, s, test, pbfile);
    }
    if (output != NULL
        && (fopen(output, "w"))
               != NULL) { //checks if the user specified an output file and the file does exist
        outfile = fopen(output, "w");
    }
    if (output == NULL) { //checks if the user didn't specify an output file
        outfile = stdout;
    }
    if (verb == 1) { //checks if verbose was turned on
        printf("user =%s \n", test);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    if (input == NULL) { //checks if an input file wasn't specified
        infile = stdin;
    }
    mpz_set_str(user, test, 62); //converts the username to an mpz_t
    if (rsa_verify(user, s, e, n) == false) { //checks if the signature is valid or not
        printf("Error, username was not verified.\n");
        exit(0);
    }
    rsa_encrypt_file(infile, outfile, n, e); //encrypts the contents of the input file
    fclose(pbfile);
    fclose(infile);
    fclose(outfile);
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(s);
    mpz_clear(user);
    mpz_clear(m);
    free(test);
    test = NULL;
    //closes and frees all files and mpz_ts
}
