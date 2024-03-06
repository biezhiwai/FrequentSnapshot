#pragma once

#include "src/include/util.h"
#include "src/include/naive.h"
#include "src/include/cou.h"
#include "src/include/zigzag.h"
#include "src/include/pingpong.h"
#include "src/include/PB.h"
#include "src/include/HG.h"
#include "src/include/mm.h"
#include "src/include/myfork.h"


typedef struct {
    int algType;
    size_t dbSize;
    int rowSize;
    int logscale_pagesize;
    struct timespec ckpTimeLog[2000];
    integer *ckpOverheadLog;
    integer *ckpPrepareLog;
    integer *ckpTotalOverheadLog;
    int ckpMaxNum;
    int ckpID;
    int dbState;
    int updateThrNum;
    int updateFrequency;
    integer globaltick;
    integer *rfBuf;
    integer rfBufSize;
    pthread_mutex_t dbStateRWLock;
    pthread_mutex_t accessMutex;
    //pthread_spinlock_t presync;
    unsigned char pre_lock;
    db_naive_infomation naiveInfo;
    db_cou_infomation couInfo;
    db_zigzag_infomation zigzagInfo;
    db_pingpong_infomation pingpongInfo;
    db_pb_infomation pbInfo;
    db_hg_infomation hgInfo;
    db_myfork_infomation myforkInfo;
    db_mm_infomation mmInfo;
    integer update_count;
    integer dbStartTime;
    integer dbEndTime;
} db_server;

//void log_time_write(db_server *s);
void add_overhead_log(db_server *s, integer ns);

void write_overhead_log(db_server *s, const char *filePath);

void add_prepare_log(db_server *s, integer ns);

void add_total_log(db_server *s, integer ns);

///////////////////////////////////////////////////////////////

typedef struct {
    size_t db_size;
    int alg_type;
    integer *random_buffer;
    int random_buffer_size;
    pthread_barrier_t *update_brr_init;
    pthread_barrier_t *brr_exit;
    int pthread_id;
    int update_frequency;
} update_thread_info;

void *update_thread(void *arg);

int update_thread_start(pthread_t *update_thread_id_array[], pthread_barrier_t *brr_exit, db_server *dbs);

int random_update_db(integer *random_buf, int buf_size, char *log_name, int uf);

int tick_update(integer *random_buf, int buf_size, int times, FILE *logFile);

void *(*db_read)(size_t index);

int (*db_write)(size_t index, void *value);

///////////////////////////////////////////////////////////////

typedef struct {
    size_t dbSize;
    int algType;
    pthread_barrier_t *ckpInitBrr;
    pthread_barrier_t *ckpExitBrr;
} db_thread_info;

void *checkpoint_thread(void *arg);

int ckp_thread_start(pthread_t *db_thread_id, pthread_barrier_t *brr_exit, db_server *dbs);

