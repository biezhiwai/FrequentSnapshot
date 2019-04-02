#include "system.h"
#include <math.h>

extern db_server DBServer;


int db_thread_start(pthread_t *db_thread_id, pthread_barrier_t *brr_exit, db_server *dbs) {
    db_thread_info dbInfo;
    pthread_barrier_t brrDBInit;
    pthread_barrier_init(&brrDBInit, NULL, 2);
    dbInfo.algType = dbs->algType;
    dbInfo.dbSize = dbs->dbSize;
    dbInfo.ckpInitBrr = &brrDBInit;
    dbInfo.ckpExitBrr = brr_exit;

    if (0 != pthread_create(db_thread_id, NULL, database_thread, &dbInfo)) {
        perror("database thread create error!");
        return -1;
    }

    pthread_barrier_wait(&brrDBInit);
    pthread_barrier_destroy(&brrDBInit);

    return 0;
}


void *database_thread(void *arg) {
    pin_To_vCPU(6);
    int dbSize = ((db_thread_info *) arg)->dbSize;
    int algType = ((db_thread_info *) arg)->algType;
    pthread_barrier_t *exitBrr = ((db_thread_info *) arg)->ckpExitBrr;
    pthread_barrier_t *initBrr = ((db_thread_info *) arg)->ckpInitBrr;

    char dbLogPath[128];
    int (*db_init)(void *, size_t);
    void (*checkpoint)(int, void *);
    void (*db_destroy)(void *);
    void *info;
#ifdef VERBOSE
    printf("database thread start alg_type:%d, dbSize:%d, unit_size:%d, set uf:%d\n",
           algType, dbSize, DBServer.unitSize, DBServer.updateFrequency);
#endif
    switch (algType) {
        case NAIVE_ALG:
            db_init = db_naive_init;
            checkpoint = ckp_naive;
            db_destroy = db_naive_destroy;
            info = &(DBServer.naiveInfo);
            snprintf(dbLogPath, sizeof(dbLogPath), "./log/naive_%d_ckp_log", dbSize);
            break;
        case COPY_ON_UPDATE_ALG:
            db_init = db_cou_init;
            checkpoint = ckp_cou;
            db_destroy = db_cou_destroy;
            info = &(DBServer.couInfo);
            snprintf(dbLogPath, sizeof(dbLogPath), "./log/cou_%d_ckp_log", dbSize);
            break;
        case ZIGZAG_ALG:
            db_init = db_zigzag_init;
            checkpoint = db_zigzag_ckp;
            db_destroy = db_zigzag_destroy;
            info = &(DBServer.zigzagInfo);
            snprintf(dbLogPath, sizeof(dbLogPath), "./log/zigzag_%d_ckp_log", dbSize);
            break;
        case PINGPONG_ALG:
            db_init = db_pingpong_init;
            checkpoint = db_pingpong_ckp;
            db_destroy = db_pingpong_destroy;
            info = &(DBServer.pingpongInfo);
            snprintf(dbLogPath, sizeof(dbLogPath), "./log/pingpong_%d_ckp_log", dbSize);
            break;
        case PB_ALG:
            db_init = db_pb_init;
            checkpoint = db_pb_ckp;
            db_destroy = db_pb_destroy;
            info = &(DBServer.pbInfo);
            snprintf(dbLogPath, sizeof(dbLogPath), "./log/mk_%d_ckp_log", dbSize);
            break;
        case HG_ALG:
            db_init = db_hg_init;
            checkpoint = db_hg_ckp;
            db_destroy = db_hg_destroy;
            info = &(DBServer.hgInfo);
            snprintf(dbLogPath, sizeof(dbLogPath), "./log/ll_%d_ckp_log", dbSize);
            break;
        case MYFORK_ALG:
            db_init = db_myfork_init;
            checkpoint = ckp_myfork;
            db_destroy = db_myfork_destroy;
            info = &(DBServer.myforkInfo);
            snprintf(dbLogPath, sizeof(dbLogPath), "./log/myfork_%d_ckp_log", dbSize);
            break;
        default:
            printf("alg_type error!");
            goto DB_EXIT;
            break;
    }

    if (0 != db_init(info, dbSize)) {
        perror("db thread init error!");
        goto DB_EXIT;
    }

    pthread_mutex_lock(&(DBServer.dbStateRWLock));
    DBServer.dbState = 1;
    pthread_mutex_unlock(&(DBServer.dbStateRWLock));
#ifdef VERBOSE
    printf("db thread init success!\n");
#endif
    pthread_barrier_wait(initBrr);
    //DBServer.isConsistent = 1;
    long long timeStart;
    long long timeEnd;
    long long timeCheckpointPeriod;
    while (1) {
        //while(0 == DBServer.isConsistent){;}
#ifdef VERBOSE
        printf("checkpoint triggered\n");
#endif
        timeStart = get_ntime();
        checkpoint(DBServer.ckpID % 2, info);
        timeEnd = get_ntime();
        add_total_log(&DBServer, timeEnd - timeStart);
        timeCheckpointPeriod = timeStart + 10 ^ 7;  // 10s
        if (timeCheckpointPeriod >= timeEnd)
            while (abs(timeCheckpointPeriod - get_ntime()) >= 100) { ; }
        DBServer.ckpID++;

        if (DBServer.ckpID >= DBServer.ckpMaxNum) {
            //pthread_mutex_lock(&(DBServer.dbStateRWLock));
            DBServer.dbState = 0;
            //pthread_mutex_unlock(&(DBServer.dbStateRWLock));
            break;
        }
    }
#ifdef VERBOSE
    printf("\ncheckpoint finish:%d\n", DBServer.ckpID);
#endif
    pthread_barrier_wait(exitBrr);

    DB_EXIT:
#ifdef VERBOSE
    printf("database thread exit\n");
#endif
    db_destroy(info);

    pthread_exit(NULL);
}

//////////////////////////////////////////////////////////////////////////


int update_thread_start(pthread_t *update_thread_id_array[],
                        pthread_barrier_t *brr_exit,
                        db_server *dbs) {

    int i;
    update_thread_info update_info;
    pthread_barrier_t update_brr_init;

    update_info.alg_type = dbs->algType;
    update_info.db_size = dbs->dbSize;
    update_info.random_buffer = dbs->rfBuf;
    update_info.random_buffer_size = dbs->rfBufSize;
    pthread_barrier_init(&update_brr_init, NULL, dbs->updateThrNum + 1);
    update_info.update_brr_init = &update_brr_init;
    update_info.brr_exit = brr_exit;
    update_info.update_frequency = dbs->updateFrequency;

    if (NULL == ((*update_thread_id_array)
                         = (pthread_t *) malloc(sizeof(pthread_t) * dbs->updateThrNum))) {
        perror("update thread array malloc error");
    }
    for (i = 0; i < dbs->updateThrNum; i++) {
        update_info.pthread_id = i;
        if (0 != pthread_create(&((*update_thread_id_array)[i]),
                                NULL, update_thread, &update_info)) {
#ifdef VERBOSE
            printf("update thread %d create error", i);
#endif
        } else {
#ifdef VERBOSE
            printf("update thread %d create success\n", i);
#endif
        }
    }

    pthread_barrier_wait(&update_brr_init);
    pthread_barrier_destroy(&update_brr_init);
    return 0;
}


void *update_thread(void *arg) {
    pin_To_vCPU(0);
    int alg_type = ((update_thread_info *) arg)->alg_type;
    long *random_buffer = ((update_thread_info *) arg)->random_buffer;
    int random_buffer_size = ((update_thread_info *) arg)->random_buffer_size;
    pthread_barrier_t *update_brr_init = ((update_thread_info *) arg)->update_brr_init;
    pthread_barrier_t *brr_exit = ((update_thread_info *) arg)->brr_exit;
    int pthread_id = ((update_thread_info *) arg)->pthread_id;
    int update_frequency = ((update_thread_info *) arg)->update_frequency;
    char log_name[128];

    switch (alg_type) {
        case NAIVE_ALG:
            db_write = naive_write;
            db_read = naive_read;
            //    snprintf(log_name,sizeof(log_name),"./log/naive_update_log_%d",pthread_id);
            break;
        case COPY_ON_UPDATE_ALG:
            db_write = cou_write;
            db_read = cou_read;
            //    snprintf(log_name,sizeof(log_name),"./log/cou_update_log_%d",pthread_id);
            break;
        case ZIGZAG_ALG:
            db_write = zigzag_write;
            db_read = zigzag_read;
            //    snprintf(log_name,sizeof(log_name),"./log/zigzag_update_log_%d",pthread_id);
            break;
        case PINGPONG_ALG:
            db_write = pingpong_write;
            db_read = pingpong_read;
            //    snprintf(log_name,sizeof(log_name),"./log/pingpong_update_log_%d",pthread_id);
            break;
        case PB_ALG:
            db_write = mk_write;
            db_read = mk_read;
            //    snprintf(log_name,sizeof(log_name),"./log/mk_update_log_%d",pthread_id);
            break;
        case HG_ALG:
            db_write = ll_write;
            db_read = ll_read;
            break;
        case MYFORK_ALG:
            db_write = myfork_write;
            db_read = myfork_read;
            //    snprintf(log_name,sizeof(log_name),"./log/myfork_update_log_%d",pthread_id);
            break;
        default:
            perror("alg_type error");
            break;
    }
    sprintf(log_name, "./log/latency/%d_latency_%dk_%ld_%d_%d.log", DBServer.algType,
            DBServer.updateFrequency / 1000, DBServer.dbSize, DBServer.unitSize,
            pthread_id);
    pthread_barrier_wait(update_brr_init);

    random_update_db(random_buffer, random_buffer_size, log_name, update_frequency);

    pthread_barrier_wait(brr_exit);

    pthread_exit(NULL);
}


// update执行的更新函数
int random_update_db(long *random_buf, int buf_size, char *log_name, int uf) {
    long long tick = 0;
    FILE *logFile = fopen(log_name, "w+");

    int isEnd;
    while (1) {
        isEnd = tick_update(random_buf, buf_size, uf, logFile, tick);
        if (-1 == isEnd)
            break;
        tick++;
    }
    fclose(logFile);
#ifdef VERBOSE
    printf("tick = %lld\n", DBServer.globaltick);
#endif
    return 0;
}


int tick_update(long *random_buf, int buf_size, int times, FILE *logFile, int tick) {
    size_t index;
    size_t *rows = (size_t *) malloc(sizeof(size_t) * 1);
    long long timeStart;
    long long timeBegin;
    long long timeEnd;
    long long timeTick;
    int i;
    long long tick_start_index = (tick % 10) * times;
    timeBegin = get_ntime();
    //pthread_spin_lock(&(DBServer.presync));
    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    timeTick = timeStart + 100000000;  // 0.1s
    i = 0;

#ifdef TICK_UPDATE
    while (i < times) {
        if (1 != DBServer.dbState) {
#ifdef VERBOSE
            printf("update thread prepare to exit\n");
#endif
            pthread_mutex_unlock(&(DBServer.dbStateRWLock));
            return -1;
        }
        index = random_buf[tick_start_index + i];
        for (int j = 0; j < 1; ++j) {
            rows[j]=index;
        }
        db_write(index, rows);
        i++;
        DBServer.update_count++;
    }
    timeEnd = get_ntime();
    if (timeTick > timeEnd)   // wait loop , with litter error
        while(get_ntime()<timeTick){;}
#elif FULL_UPDATE
    while(1)
    {
        if (1 != DBServer.dbState) {
                printf("update thread prepare to exit\n");
                pthread_mutex_unlock(&(DBServer.dbStateRWLock));
                return -1;
        }

        index = random_buf[tick + i];
        db_write(index , random_buf + tick);
        i++;
        DBServer.update_count++;
        if(abs(timeTick - get_ntime()) <= 100000)
            break;
    }
    timeEnd = get_ntime();
#endif
    //pthread_spin_unlock(&(DBServer.presync));
    free(rows);
    db_unlock(&(DBServer.pre_lock));
    DBServer.globaltick++;
    fprintf(logFile, "%lld\t%lld\n", timeBegin, (timeEnd - timeBegin));
    return 0;
}


void add_overhead_log(db_server *s, long long ns) {
    s->ckpOverheadLog[s->ckpID] = ns;
}

void add_prepare_log(db_server *s, long long ns) {
    s->ckpPrepareLog[s->ckpID] = ns;
}

void add_total_log(db_server *s, long long ns) {
    s->ckpTotalOverheadLog[s->ckpID] = ns;
}

void write_overhead_log(db_server *s, const char *filePath) {
    FILE *logFile;
    int i;
    logFile = fopen(filePath, "w");

    for (i = 0; i < s->ckpID; i++) {
        fprintf(logFile, "%lld\t%lld\t%lld\n", s->ckpPrepareLog[i],
                s->ckpOverheadLog[i], s->ckpTotalOverheadLog[i]);
    }
    fflush(logFile);
    fclose(logFile);
}
