#pragma once
#include "util.h"
#include "naive.h"
#include "cou.h"
#include "zigzag.h"
#include "pingpong.h"
#include "mk.h"
#include "LL.h"



typedef struct {
    int algType;
    int dbSize;
    int unitSize;
    struct timespec ckpTimeLog[2000];
    long long *ckpOverheadLog;
    long long *ckpPrepareLog;
    long long *ckpTotalOverheadLog;
    int ckpMaxNum;
    int ckpID;
    int dbState;
    int updateThrNum;
    int updateFrequency;
	int isConsistent;
    long long  globaltick;
    int *rfBuf;
    int rfBufSize;
    pthread_mutex_t dbStateRWLock;
    pthread_mutex_t accessMutex;
    pthread_spinlock_t presync;
    db_naive_infomation naiveInfo;
    db_cou_infomation couInfo;
    db_zigzag_infomation zigzagInfo;
    db_pingpong_infomation pingpongInfo;
    db_mk_infomation mkInfo;
    db_ll_infomation llInfo;
    long long update_count;
    long long dbStartTime;
    long long dbEndTime;
} db_server;

//void log_time_write(db_server *s);
void add_overhead_log(db_server *s,long long ns);
void write_overhead_log(db_server *s,const char *filePath);
void add_prepare_log(db_server *s,long long ns);
void add_total_log(db_server *s,long long ns);

///////////////////////////////////////////////////////////////

typedef struct {
    int db_size;
    int alg_type;
    int *random_buffer;
    int random_buffer_size;
    pthread_barrier_t *update_brr_init;
    pthread_barrier_t *brr_exit;
    int pthread_id;
    int update_frequency;
}update_thread_info;

void *update_thread(void *arg);
int update_thread_start(pthread_t *update_thread_id_array[],pthread_barrier_t *brr_exit,db_server *dbs);
int random_update_db( int *random_buf,int buf_size,char *log_name,int uf);
int tick_update(int *random_buf, int buf_size, int times, FILE *logFile, int tick);
void* (*db_read)(int index);
int (*db_write)(int index, void* value);

///////////////////////////////////////////////////////////////

typedef struct {
    int dbSize;
    int algType;
    pthread_barrier_t *ckpInitBrr;
    pthread_barrier_t *ckpExitBrr;
} db_thread_info;

void *database_thread(void *arg);
int db_thread_start(pthread_t *db_thread_id, pthread_barrier_t *brr_exit, db_server *dbs);

