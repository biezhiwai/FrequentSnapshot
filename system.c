#include "system.h"
#include <math.h>

extern db_server DBServer;

int random_update_db(int *random_buf, int buf_size, char *log_name, int uf)
{
	long long tick = 0;
	FILE *logFile = fopen(log_name, "w+");

	//long long timeStartNs;
	//long long timeEndNs;
	//long long timeBeginNs;
	//long long timeDiff;

	int isEnd;
	//timeBeginNs = get_ntime();
	while (1) {
		//timeStartNs = get_ntime();
		isEnd = tick_update(random_buf, buf_size, uf , logFile, tick);
		//timeEndNs = get_ntime();
		if (-1 == isEnd)
			break;
		//next 10ms tick
		tick++;
	}
	fclose(logFile);
	pthread_mutex_lock(&(DBServer.accessMutex));
	pthread_mutex_unlock(&(DBServer.accessMutex));
	//tick = tick * uf + i * (uf / 100);
	//time_now_us = time_now.tv_sec * 1000000 + time_now.tv_nsec / 1000;
	printf("tick = %lld\n" , DBServer.globaltick);
	//timeDiff = (timeEndNs - timeBeginNs) / 1000000;
	//real uf is the throughput
	//printf("set uf:%d,real uf:%ld\n", uf, timeDiff == 0 ? 0 : tick / timeDiff);
	return 0;
}

int tick_update(int *random_buf, int buf_size, int times, FILE *logFile, int tick)
{
    int index;
    long long timeStart;
	long long timeBegin;
    long long timeEnd;
    long long timeTick;
    int i;
	DBServer.isConsistent = 0;
	
    timeBegin = get_utime();
    pthread_spin_lock(&(DBServer.presync));
	timeStart = get_utime();
    timeTick = timeStart + 1000 * 100;  // 100ms
    i = 0;
	#ifdef TICK_UPDATE
	while(i < times){
		if (1 != DBServer.dbState) {
				printf("update thread prepare to exit\n");
				pthread_mutex_unlock(&(DBServer.dbStateRWLock));
				return -1;
		}

		index = random_buf[tick + i];
		db_write(index , random_buf + tick);
		i++;
		DBServer.update_count++;
	}
	timeEnd = get_utime();
	if(timeTick>timeEnd)   // wait loop , error of 0.5ms
		while(abs(timeTick- get_utime()) >= 50) {;}
	else{
		printf("update rate is so high\n");
		return -1;
	}
	
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
		if(abs(timeTick - get_utime()) <= 50) 
			break;
	}
	timeEnd = get_utime();
	#endif
    pthread_spin_unlock(&(DBServer.presync));
	DBServer.isConsistent = 1;
    DBServer.globaltick++;
    fprintf(logFile, "%lld,%lld\n",timeStart,(timeEnd - timeBegin));
    return 0;
}


void *database_thread(void *arg)
{
	pin_To_vCPU(6);
	int dbSize = ((db_thread_info *) arg)->dbSize;
	int algType = ((db_thread_info *) arg)->algType;
	pthread_barrier_t *exitBrr = ((db_thread_info *) arg)->ckpExitBrr;
	pthread_barrier_t *initBrr = ((db_thread_info *) arg)->ckpInitBrr;

	char dbLogPath[128];
	int (*db_init)(void *, int);
	void (*checkpoint)(int, void *);
	void (*db_destroy)(void *);
	void *info;

	printf("database thread start dbSize:%d alg_type:%d,unit_size:%d,set uf:%d\n",
		dbSize, algType,DBServer.unitSize,DBServer.updateFrequency);
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
	case MK_ALG:
		db_init = db_mk_init;
		checkpoint = db_mk_ckp;
		db_destroy = db_mk_destroy;
		info = &(DBServer.mkInfo);
		snprintf(dbLogPath, sizeof(dbLogPath), "./log/mk_%d_ckp_log", dbSize);
		break;
	case LL_ALG:
		db_init = db_ll_init;
		checkpoint = db_ll_ckp;
		db_destroy = db_ll_destroy;
		info = &(DBServer.llInfo);
		snprintf(dbLogPath, sizeof(dbLogPath), "./log/ll_%d_ckp_log", dbSize);
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

	printf("db thread init success!\n");
	pthread_barrier_wait(initBrr);
	DBServer.isConsistent = 1;
	long long timeStart;
	long long timeEnd;
	long long timeCheckpointPeriod;
	while (1) {
		while(0 == DBServer.isConsistent){;}
        timeStart = get_utime();
        printf("checkpoint timestamp:%ds\n", (int) (timeStart / 1000000));
        checkpoint(DBServer.ckpID % 2, info);
		timeEnd = get_utime();
        add_total_log( &DBServer, timeEnd - timeStart);
		timeCheckpointPeriod = timeStart + 10000000;
		if(timeCheckpointPeriod >=timeEnd)
			while(abs(timeCheckpointPeriod - get_utime()) >= 80) {;}
        DBServer.ckpID++;
		
        if (DBServer.ckpID  >= DBServer.ckpMaxNum) {
   	    	pthread_mutex_lock(&(DBServer.dbStateRWLock));
      		DBServer.dbState = 0;
        	pthread_mutex_unlock(&(DBServer.dbStateRWLock));
        	break;
        }
	}
	printf("\ncheckpoint finish:%d\n", DBServer.ckpID);
	pthread_barrier_wait(exitBrr);

DB_EXIT:
	printf("database thread exit\n");
	db_destroy(info);

	pthread_exit(NULL);
}

int db_thread_start(pthread_t *db_thread_id, pthread_barrier_t *brr_exit, db_server *dbs)
{
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

int update_thread_start(pthread_t *update_thread_id_array[],
    pthread_barrier_t *brr_exit,
    db_server *dbs)
{

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
            printf("update thread %d create error", i);
        } else {
            printf("update thread %d create success\n", i);
        }
    }
    pthread_barrier_wait(&update_brr_init);
    pthread_barrier_destroy(&update_brr_init);
    return 0;
}


void *update_thread(void *arg)
{
	pin_To_vCPU(0);
	int alg_type = ((update_thread_info *) arg) ->alg_type;
	int *random_buffer = ((update_thread_info *) arg) ->random_buffer;
	int random_buffer_size = ((update_thread_info *) arg) ->random_buffer_size;
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
	case MK_ALG:
		db_write = mk_write;
		db_read = mk_read;
		//    snprintf(log_name,sizeof(log_name),"./log/mk_update_log_%d",pthread_id);
		break;
	case LL_ALG:
		db_write = ll_write;
		db_read = ll_read;
		break;
	default:
		perror("alg_type error");
		break;
	}
	sprintf(log_name, "./log/latency/%d_latency_%dk_%d_%d_%d.log", DBServer.algType,
		DBServer.updateFrequency / 1000, DBServer.dbSize, DBServer.unitSize,
		pthread_id);
	pthread_barrier_wait(update_brr_init);

	random_update_db(random_buffer, random_buffer_size, log_name, update_frequency);

	pthread_barrier_wait(brr_exit);

	pthread_exit(NULL);
}



void log_time_write(db_server *s)
{
	FILE *log_time;
	int i;
	char logName[256];
	long long timeStart;
	long long timeEnd;

	long long timeSum = 0;
	sprintf(logName,"./log/overhead/%d_overhead_%dk_%d_%d.log",
		DBServer.algType,DBServer.updateFrequency,
		DBServer.dbSize,DBServer.unitSize);
	if (NULL == (log_time = fopen(logName, "w"))) {
		perror("log_time fopen error,checkout if the floder is exist");
		return;
	}
	for (i = 1; i < s->ckpMaxNum; i++) {

		timeStart = s->ckpTimeLog[i * 2].tv_sec * 1000000000 +
			s->ckpTimeLog[i*2].tv_nsec;
		timeEnd = s->ckpTimeLog[i * 2 + 1].tv_sec * 1000000000 +
			s->ckpTimeLog[i*2 + 1].tv_nsec;
		timeSum += timeEnd - timeStart;
	}
	fprintf(log_time,"%lld\n",timeSum/ (s->ckpMaxNum - 1));
	fflush(log_time);
	fclose(log_time);
}
void add_overhead_log(db_server *s,long long ns)
{
    s->ckpOverheadLog[s->ckpID] = ns;
}
void add_prepare_log(db_server *s,long long ns)
{
    s->ckpPrepareLog[s->ckpID] = ns;
}
void add_total_log(db_server *s,long long ns)
{
    s->ckpTotalOverheadLog[s->ckpID] = ns;
}
void write_overhead_log(db_server *s,const char *filePath)
{
    FILE *logFile;
    int i;
    logFile = fopen(filePath,"w");

    fprintf(logFile,"Prepare\t\t\tOverhead\t\ttotal\n");
    for (i = 0; i < s->ckpID; i ++){
        fprintf(logFile,"%lld\t\t%lld\t\t%lld\n",s->ckpPrepareLog[i],
            s->ckpOverheadLog[i],s->ckpTotalOverheadLog[i]);
    }
    fflush(logFile);
    fclose(logFile);
}
