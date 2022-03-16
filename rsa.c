#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
//#include <stdlib.h>
//#include <gmp.h>
//lines 39 to 42 were derived from Elmer(TA); https://discord.com/channels/926211705766305842/9262117063828687/940378247877767220
//lines 115 to 121 were derived from Eugene Chou(TA) in Section/Office Hours
//lines 148 to 154 were derived from Eugene Chou(TA) in Section/Office Hours
//line 151 derived from Simon Kwong(Student in 13s Winter Quarter)
void lcm(mpz_t d, mpz_t a, mpz_t b);
//void mpz_log(mpz_t rop, mpz_t base, mpz_t exponent);

//

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits,
    uint64_t iters) { //function that makes the public key
    mpz_t p_minus;
    mpz_t q_minus;
    mpz_t least;
    mpz_t tester;

    mpz_init(q_minus);
    mpz_init(p_minus);
    mpz_init(tester);
    mpz_init(least);
    //declaration of temporary mpz_ts
    uint64_t size = (random() % (3 * nbits / 4 - nbits / 4 + 1)) + nbits / 4;
    uint64_t remain = nbits - size;
    //calculates the respective bit sizes for p and q
    make_prime(p, size, iters);
    make_prime(q, remain, iters);
    mpz_mul(n, p, q);
    //generates p and q and then calculates n by taking the product of p and q
    mpz_sub_ui(q_minus, q, 1);
    mpz_sub_ui(p_minus, p, 1);

    lcm(least, p_minus, q_minus);
    //finds the lcm of p-1 and q-1 in order to find the coprimes of p and q
    do { //do while loop that generates the public key between 0 and 2^bits-1 long
        mpz_urandomb(e, state, nbits);
        gcd(tester, e, least);
    } while (mpz_cmp_ui(tester, 1) != 0); //loops again if gcd(e, lcm(p-1, q-1)) is not 0

    mpz_clear(q_minus);
    mpz_clear(p_minus);
    mpz_clear(tester);
    mpz_clear(least);
    //clears all mpz_ts
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx \n", n);
    gmp_fprintf(pbfile, "%Zx \n", e);
    gmp_fprintf(pbfile, "%Zx \n", s);
    fprintf(pbfile, "%s \n", username);
    //uses gmp_fprintf and fprintf in order to print out n, e, s as hexstrings and username as a regular string
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx \n", n);
    gmp_fscanf(pbfile, "%Zx \n", e);
    gmp_fscanf(pbfile, "%Zx \n", s);
    fscanf(pbfile, "%s \n", username);
    //reads n, e, s as numbers and username as a string using gmp_fscanf and fscanf respectively
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t lambda;
    mpz_t p_minus;
    mpz_t q_minus;

    mpz_init(lambda);
    mpz_init(p_minus);
    mpz_init(q_minus);

    mpz_sub_ui(p_minus, p, 1);
    mpz_sub_ui(q_minus, q, 1);
    lcm(lambda, p_minus, q_minus);
    mod_inverse(d, e, lambda);
    mpz_clear(lambda);
    mpz_clear(p_minus);
    mpz_clear(q_minus);
    //generates the private key by getting the lcm(p-1, q-1) and finding the modular inverse of e, the public key
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx \n", n);
    gmp_fprintf(pvfile, "%Zx \n", d);
    //writes n and d to the private key file using gmp_fprintf
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx \n", n);
    gmp_fscanf(pvfile, "%Zx \n", d);
    //reads n and d from the private key file using gmp_fscanf
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    //encryption using C = M^e mod n
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    uint64_t k;
    size_t j;
    mpz_t m;
    mpz_t c;
    //mpz_t log_minus;

    k = (mpz_sizeinbase(n, 2) - 1) / 8;
    //the interval at which bytes are read in the file
    //mpz_sub_ui(log_minus, log_minus, 1);
    mpz_init(m);
    mpz_init(c);
    //intialization of the plaintext and ciphertext mpz_ts
    uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));
    arr[0] = 0xFF; //declaration of array with prepended 0xFF
    while ((j = fread(&arr[1], sizeof(uint8_t), k - 1, infile))
           > 0) { //uses j to check if there are still bytes to be read in the file
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, arr); //converts the elements in arr to m
        rsa_encrypt(c, m, e, n); //calls rsa_encrypt to get c
        gmp_fprintf(outfile, "%Zx \n", c); //prints c to the outfile(default is stdout)
    }
    free(arr);
    mpz_clear(m);
    mpz_clear(c);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    //decryption using M = C^d mod n
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    uint64_t k;
    size_t j;
    mpz_t c;
    mpz_t m;
    //mpz_t log_minus;

    k = (mpz_sizeinbase(n, 2) - 1) / 8;
    //the interval at which the plaintext and ciphertext are read in the file
    mpz_init(c);
    mpz_init(m);
    //initialization of the plaintext and ciphertext mpz_ts
    //mpz_sub_ui(log_minus, log_minus, 1);
    //mpz_fdiv_q_ui(log_minus, log_minus, 8);
    uint8_t *arr = (uint8_t *) calloc(k, sizeof(uint8_t));

    while (gmp_fscanf(infile, "%Zx \n", c) != EOF) {
        rsa_decrypt(m, c, d, n);
        mpz_export(arr, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(&arr[1], sizeof(uint8_t), j - 1, outfile);
    }
    free(arr);
    mpz_clear(c);
    mpz_clear(m);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    //calls pow_mod in order get the signature
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    pow_mod(t, s, e, n);
    if (mpz_cmp(m, t) == 0) {
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
    mpz_clear(t);
    //verifys that the signature is valid using T = S^e mod n
}

void lcm(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t product;
    mpz_t x;

    mpz_t temp, temp2;

    mpz_init(x);
    mpz_init(product);
    mpz_init(temp);
    mpz_init(temp2);

    mpz_set(temp, a);
    mpz_set(temp2, b);

    mpz_mul(product, temp, temp2);
    mpz_abs(product, product);
    gcd(x, a, b);

    mpz_div(d, product, x);
    mpz_clear(x);
    mpz_clear(product);
    mpz_clear(temp);
    mpz_clear(temp2);
    //lcm that uses the gcd function to simulate abs(a*b)/gcd(a, b)
}
