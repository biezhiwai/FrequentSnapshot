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


