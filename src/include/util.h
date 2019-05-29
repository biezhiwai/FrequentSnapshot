#pragma once
#define UNLOCK  0
#define LOCK    -1

#define NAIVE_ALG           0
#define COPY_ON_UPDATE_ALG  1
#define MYFORK_ALG          2
#define ZIGZAG_ALG          3
#define PINGPONG_ALG        4
#define HG_ALG              5
#define PB_ALG              6



#define CHECKPOINT_COUNT 5
#define FILED_SIZE 8
#include <stdio.h>
#include <stdbool.h>
#include<sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<fcntl.h>
#include <time.h>
#include <numa.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>


#include <sched.h>
#include <pthread.h>

typedef long long int integer;

integer get_ntime(void);

integer get_utime(void);

integer get_mtime(void);

void db_lock(unsigned char *lock);

void db_unlock(unsigned char *lock);

int pin_To_vCPU(int cpu);

