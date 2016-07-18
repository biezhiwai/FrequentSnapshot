#include "util.h"

long long get_ntime(void)
{
	struct timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	return timeNow.tv_sec * 1000000000 + timeNow.tv_nsec;
}

long long get_utime(void)
{
	return get_ntime() / 1000;
}

long long get_mtime(void)
{
	return get_ntime() / 1000000;
}

int randomfile_init(FILE *rf,int *rbuf,long long rbufSize)
{
    long long i;

    for (i = 0; i < rbufSize; i ++){
        fscanf(rf,"%d\n",rbuf + i);
    }
    return i;
}

int pin_To_vCPU(int cpu)
{
    cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpu,&mask);
    if(pthread_setaffinity_np( pthread_self() , sizeof(mask),&mask) < 0)
	    return 0;
    return 1;
}


void db_lock(unsigned char *lock)
{
    char expected = UNLOCK;
    while( !__atomic_compare_exchange_1(lock,&expected,LOCK,0,__ATOMIC_RELAXED,__ATOMIC_RELAXED)){
        expected = UNLOCK;
    }
}

void db_unlock(unsigned char *lock)
{
    __atomic_store_1(lock,UNLOCK,__ATOMIC_RELAXED);
}
