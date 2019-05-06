#include "util.h"
#include <math.h>
integer get_ntime(void) {
    struct timespec timeNow;
    clock_gettime(CLOCK_MONOTONIC, &timeNow);
    return timeNow.tv_sec * 1000000000 + timeNow.tv_nsec;
}

integer get_utime(void){
    return get_ntime() / 1000;
}

integer get_mtime(void) {
    return get_ntime() / 1000000;
}

int pin_To_vCPU(int cpu) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
        return 0;
    return 1;
}


void db_lock(unsigned char *lock) {
    char expected = UNLOCK;
    while (!__atomic_compare_exchange_1(lock, &expected, LOCK, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
        expected = UNLOCK;
    }
}

void db_unlock(unsigned char *lock) {
    __atomic_store_1(lock, UNLOCK, __ATOMIC_RELAXED);
}

double rand_val(int seed) {
    const long a = 16807;  // Multiplier
    const long m = 2147483647;  // Modulus
    const long q = 127773;  // m div a
    const long r = 2836;  // m mod a
    static long x;               // Random int value
    long x_div_q;         // x divided by q
    long x_mod_q;         // x modulo q
    long x_new;           // New x value

    // Set the seed if argument is non-zero and then return zero
    if (seed > 0) {
        x = seed;
        return (0.0);
    }

    // RNG using integer arithmetic
    x_div_q = x / q;
    x_mod_q = x % q;
    x_new = (a * x_mod_q) - (r * x_div_q);
    if (x_new > 0)
        x = x_new;
    else
        x = x_new + m;

    // Return a random value between 0.0 and 1.0
    return ((double) x / m);
}


int zipf(double alpha, int n) {
    static int first = 1;      // Static first time flag
    static double c = 0;          // Normalization constant
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value;            // Computed exponential value to be returned
    int i;                     // Loop counter

    // Compute normalization constant on first call only
    if (first == 1) {
        for (i = 1; i <= n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = 0;
    }

    // Pull a uniform random number (0 < z < 1)
    do {
        z = rand_val(0);
    } while ((z == 0) || (z == 1));

    // Map z to the value
    sum_prob = 0;
    for (i = 1; i <= n; i++) {
        sum_prob = sum_prob + c / pow((double) i, alpha);
        if (sum_prob >= z) {
            zipf_value = i;
            break;
        }
    }

    // Assert that zipf_value is between 1 and N
    //assert((zipf_value >=1) && (zipf_value <= n));

    return (zipf_value);
}

