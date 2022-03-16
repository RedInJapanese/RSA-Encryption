#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <gmp.h>

#include "rsa.h"
#include "randstate.h"

#define OPTIONS "b:i:n:d:s:vh"

//implementation of lines 78 to 79 derived from Simon Kwong(Student 13s Winter Quarter)
void usage(char *
        exec) { //function that prints out the usage of the keygen executable whenever it is ran with -h
    fprintf(stderr,
        "SYNOPSIS\n"
        "	Generates an RSA public/private key pair.\n"
        "\n"
        "USAGE\n"
        "	%s [OPTIONS]\n"
        "OPTIONS\n"
        "	-h		Display program help and usage.\n"
        "	-v		Display verbose program output.\n"
        "	-b bits		Minimum bits needed for public key n(default 256).\n"
        "	-i iterations	Miller-Rabin iterations for testing primes default(default: 50).\n"
        "	-n pbfile	Public key file (default: rsa.pub)\n"
        "	-d pvfile	Private key file(default: rsa.priv).\n"
        "	-s seed 	Random seed for testing\n",
        exec);
}
int main(int argc, char **argv) {
    int opt = 0;
    uint64_t bits = 256;
    uint64_t iters = 50;
    bool check_seed = false;
    int seed = 0;
    int verb = 0;
    char *pub = "rsa.pub";
    char *priv = "rsa.priv";
    FILE *pbfile = NULL;
    FILE *pvfile = NULL;
    char *user = getenv("USER");
    mpz_t use;
    mpz_t signature;
    //declaration of variables that are going to be used based on the input in the command line from the user
    mpz_t p, q, n, e, d;
    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);
    mpz_init(use);
    mpz_init(signature);
    //initialization of all mpz_ts

    while ((opt = getopt(argc, argv, OPTIONS))
           != -1) { //switch statement for all the different command line options
        switch (opt) {
        case 'b': bits = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'i': iters = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'n': pub = optarg; break;
        case 'd': priv = optarg; break;
        case 's':
            seed = (uint64_t) strtoul(optarg, NULL, 10);
            check_seed = true;
            break;
        case 'v': verb = 1; break;
        case 'h': usage(argv[0]); break;
        }
    }

    pbfile = fopen(pub, "w");
    pvfile = fopen(priv,
        "w"); //opens the private and public key files for writing based on what the user inputs

    uint64_t fi = fileno(pvfile);
    fchmod(fi, S_IRUSR | S_IWUSR); //gets the username so as to verify who's using the keys

    if (check_seed == false) {
        randstate_init(time(NULL)); //initialization of seed
    } else {
        randstate_init(seed);
    }
    if (bits <= 0) {
        exit(0);
    }
    rsa_make_pub(p, q, n, e, bits, iters);
    rsa_make_priv(d, e, p, q);

    mpz_set_str(use, user, 62); //converts the username into an mpz_t with a base of 62
    rsa_sign(signature, use, d, n);
    rsa_write_pub(n, e, signature, user, pbfile);
    rsa_write_priv(n, d, pvfile);
    //creates the public and private keys using the rsa functions made beforehand

    if (verb == 1) { //prints the variables and their bit sizes if verbose is turned on
        printf("user = %s \n", user);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(signature, 2), signature);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(n);
    mpz_clear(e);
    mpz_clear(d);
    mpz_clear(use);
    mpz_clear(signature);
    fclose(pvfile);
    fclose(pbfile);
    randstate_clear();
    //frees all mpz_ts and closes all files
}
