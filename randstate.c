#include "randstate.h"
#include <stdio.h>
#include <stdlib.h>

gmp_randstate_t state; //global variable 'state' created here

void randstate_init(uint64_t
        seed) { //initializes state using the seed passed as an argument using Mersenne Twister algorithm
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    srandom(seed);
}
void randstate_clear(void) { //frees state
    gmp_randclear(state);
}
