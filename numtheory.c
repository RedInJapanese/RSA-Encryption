#include "numtheory.h"
#include "randstate.h"

//implementation of lines 99-104 were inspired by Simon Kwong(student in 13s Winter 2022)
//implementation for line 177 and 181 was inspired by Eugene Chou(TA) in Section
void gcd(mpz_t d, mpz_t a, mpz_t b) { //gcd function that uses Euclid's algorithm
    mpz_t t;
    mpz_init(t);

    mpz_t temp;
    mpz_init(temp);
    mpz_set(temp, b);

    mpz_t temp2;
    mpz_init(temp2);
    mpz_set(temp2, a);
    //mpz_ts are also pointers, so temporary mpz_ts are initialized in order to find the gcd
    while (mpz_cmp_ui(temp, 0) != 0) {
        mpz_set(t, temp);
        mpz_mod(
            temp, temp2, temp); //the loop iterates and keeps using modulus until b is equal to 0
        mpz_set(temp2, t);
    }
    mpz_set(d, temp2); //temporary mpz_t is finally set to the output mpz_t
    mpz_clear(temp2);
    mpz_clear(temp);
    mpz_clear(t);
    //all mpz_ts are cleared
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent,
    mpz_t modulus) { //function used for modular exponentiation
    mpz_t p;
    mpz_init(p);
    mpz_set(p, base);

    mpz_t exp;
    mpz_init(exp);
    mpz_set(exp, exponent);
    mpz_t n;
    mpz_init(n);
    mpz_set(n, modulus);

    mpz_t v;
    mpz_init(v);
    mpz_set_ui(v, 1);
    //initialization of temporary variables used for calculating out = base^exponent(mod modulus)
    while (mpz_cmp_ui(exp, 0) > 0) { //conditions to check if the exponent isn't zero
        if (mpz_odd_p(exp) != 0) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n); //p^2mod n
        mpz_fdiv_q_ui(exp, exp, 2); //floor division: exp = exp/2
    }
    mpz_set(out, v);
    mpz_clear(exp);
    mpz_clear(n);
    mpz_clear(p);
    mpz_clear(v);
    //clears all mpz_ts
}

bool is_prime(mpz_t n,
    uint64_t
        iters) { //function that uses miller-rabin primality test in order to verify ifan integer is prime or not
    mpz_t dividend;
    mpz_init(dividend);

    mpz_t s;
    mpz_init(s);

    mpz_set(dividend, n);
    mpz_sub_ui(dividend, dividend, 1);

    mpz_t r;
    mpz_init(r);

    mpz_t div;
    mpz_init(div);
    mpz_set_ui(div, 1);

    mpz_t result;
    mpz_init(result);
    mpz_set(result, n);
    mpz_sub_ui(result, result, 3);

    mpz_t a;
    mpz_init(a);

    mpz_t y;
    mpz_init(y);

    mpz_t n_minus;
    mpz_init(n_minus);
    mpz_sub_ui(n_minus, n, 1);

    mpz_t s_minus;
    mpz_init(s_minus);

    mpz_t j;
    mpz_init(j);
    //temp mpz_ts declared and initialized here so as not to change the values of the original parameters
    while (mpz_odd_p(r)
           == 0) { //while loop used to find s and r by repeatedly dividing n-1 by 2 until r is odd
        mpz_add_ui(s, s, 1);
        mpz_div(r, dividend, div);
        mpz_mul_ui(div, div, 2);
    }
    mpz_sub_ui(s, s, 1);
    //gmp_printf("r %Zd s %Zd \n", r, s);

    mpz_sub_ui(s_minus, s, 1);

    for (uint64_t i = 1; i < iters;
         i++) { //iterates from 1 to the specified number by the user(default is 50)

        mpz_urandomm(
            a, state, result); //calls u_randomm which generates numbers from 0 to n-1 inclusive
        mpz_add_ui(a, a, 2); //increments a by 2 in order to set the intverval from 2 to n-2

        pow_mod(y, a, r, n); //calls pow_mod to set y to a^r(mod n)

        if (mpz_cmp_ui(y, 1) != 0
            && mpz_cmp(y, n_minus) != 0) { //conditional to check if y isn't 1 and y isn't n-1
            mpz_set_ui(j, 1); //sets j to 1

            while (
                mpz_cmp(j, s_minus) <= 0
                && mpz_cmp(y, n_minus)
                       != 0) { //while loop to check that j is less than or equal to s-1 and y isn't equal to n-1
                mpz_mul(y, y, y);
                mpz_mod(y, y, n); // y^2 mod n
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clear(dividend);
                    mpz_clear(s);
                    mpz_clear(r);
                    mpz_clear(div);
                    mpz_clear(y);
                    mpz_clear(a);
                    mpz_clear(s_minus);
                    mpz_clear(n_minus);
                    mpz_clear(j);
                    mpz_clear(result);
                    return false;
                    //returns false if y is equal to 1
                }
                mpz_add_ui(j, j, 1);
                //increments j by 1
            }
            if (mpz_cmp(y, n_minus) != 0) {
                mpz_clear(dividend);
                mpz_clear(s);
                mpz_clear(r);
                mpz_clear(div);
                mpz_clear(y);
                mpz_clear(a);
                mpz_clear(s_minus);
                mpz_clear(n_minus);
                mpz_clear(j);
                mpz_clear(result);
                return false;
                //returns false if y is not equal to n-1
            }
        }
    }
    mpz_clear(dividend);
    mpz_clear(s);
    mpz_clear(r);
    mpz_clear(div);
    mpz_clear(y);
    mpz_clear(a);
    mpz_clear(s_minus);
    mpz_clear(n_minus);
    mpz_clear(j);
    mpz_clear(result);
    //clears all mpz_ts
    return true;
}

void make_prime(
    mpz_t p, uint64_t bits, uint64_t iters) { //function used to generate random prime numbers
    mpz_t min;
    mpz_init(min);

    mpz_ui_pow_ui(min, 2, bits);

    mpz_urandomb(p, state, bits); //generates a random prime number between 0 and 2^bits-1 long
    mpz_add(p, p,
        min); //adds 2^bits to the result to make sure the number is at least bits number of bits long
    while (is_prime(p, iters)
           == false) { // while loop to check if the randomly generated number is prime or not
        //gmp_printf("make prime %Zd\n", p);
        mpz_urandomb(
            p, state, bits); //keep making numbers that are bits long until the number is prime
        mpz_add(p, p, min);
    }
    mpz_clear(min);
    //clears all mpz_ts
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) { //function to find the modular inverse
    mpz_t r, r_prime;
    mpz_t t, t_prime;
    mpz_t q, q_product_r, q_product_t;
    mpz_t r_sub;
    mpz_t t_sub;

    mpz_init(r);
    mpz_init(r_prime);
    mpz_init(t);
    mpz_init(t_prime);
    mpz_init(q);
    mpz_init(q_product_r);
    mpz_init(q_product_t);
    mpz_init(r_sub);
    mpz_init(t_sub);

    mpz_set(r, n);
    mpz_set(r_prime, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);

    mpz_t temp_r;
    mpz_init(temp_r);
    mpz_t temp_i;
    mpz_init(temp_i);
    //declaration of temporary mpz_ts

    while (mpz_cmp_ui(r_prime, 0) != 0) { //while loop to check if r' is 0
        mpz_fdiv_q(q, r, r_prime);

        mpz_mul(q_product_r, q, r_prime);
        mpz_mul(q_product_t, q, t_prime);

        mpz_sub(r_sub, r, q_product_r);
        mpz_sub(t_sub, t, q_product_t);

        mpz_set(temp_r, r_prime);
        mpz_set(temp_i, t_prime);

        mpz_set(r, temp_r);
        mpz_set(r_prime, r_sub);
        mpz_set(t, temp_i);
        mpz_set(t_prime, t_sub);
        //math operations to simulate q = floor(r/r'), and the switching of r and r'
    }
    if (mpz_cmp_ui(r, 1) > 0) { //returns no inverse if r is greater than 1
        mpz_clear(r);
        mpz_clear(r_prime);
        mpz_clear(q);
        mpz_clear(t_prime);
        mpz_clear(q_product_r);
        mpz_clear(q_product_t);
        mpz_clear(r_sub);
        mpz_clear(t_sub);
        mpz_clear(temp_r);
        mpz_clear(temp_i);
        mpz_clear(t);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) { //increments t by n if its less than 0
        mpz_add(t, t, n);
    }

    mpz_set(i, t);

    mpz_clear(r);
    mpz_clear(r_prime);
    mpz_clear(q);
    mpz_clear(t_prime);
    mpz_clear(q_product_r);
    mpz_clear(q_product_t);
    mpz_clear(r_sub);
    mpz_clear(t_sub);
    mpz_clear(temp_r);
    mpz_clear(temp_i);
    mpz_clear(t);
    //clears all mpz_ts
}
