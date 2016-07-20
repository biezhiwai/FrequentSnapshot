#include "util.h"
#include "system.h"

db_server DBServer;

int main(int argc, char *argv[])
{
    int i;
    FILE *rf;
    pthread_t *update_thread_array;
    pthread_t db_thread_id;
    pthread_barrier_t brr_exit;
    char logName[128];
    if (argc != 7)
    {
        perror("usage:./ckp_cimulator [update thread number] [unit num] "
               "[algorithm type:0-navie 1-copy on update 2-zigzag 3-pingpong] "
               "[random file name] [update frequency (k/sec)]"
               "[unit size]");
    }
    DBServer.updateThrNum = atoi(argv[1]);
    DBServer.dbSize = atoi(argv[2]);
    DBServer.algType = atoi(argv[3]);
    if (NULL == (rf = fopen(argv[4], "r")))
    {
        perror("random file open error!\n");
        return -1;
    }
    DBServer.updateFrequency = atoi(argv[5]) * 1000;
    DBServer.unitSize = atoi(argv[6]);
    DBServer.ckpID = 0;
    DBServer.dbState = 0;
    DBServer.ckpMaxNum = 10;
	DBServer.update_count = 0;
    DBServer.ckpOverheadLog = malloc( sizeof(long long) * DBServer.ckpMaxNum);
    DBServer.ckpPrepareLog = malloc(sizeof(long long) * DBServer.ckpMaxNum);
    DBServer.ckpTotalOverheadLog = malloc(sizeof(long long) * DBServer.ckpMaxNum);

    DBServer.globaltick = 0;
    DBServer.rfBufSize = DBServer.updateFrequency * 10;
    DBServer.rfBuf = (long *) malloc(DBServer.rfBufSize * sizeof(long));
    if (DBServer.rfBufSize != randomfile_init(rf,DBServer.rfBuf,DBServer.rfBufSize))
    {
        perror("random file init error\n");
        return -1;
    }
    fclose(rf);
    pthread_spin_init(&(DBServer.presync) ,PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(DBServer.accessMutex),NULL);
    pthread_mutex_init(&(DBServer.dbStateRWLock),NULL);

    pthread_barrier_init(&brr_exit, NULL, DBServer.updateThrNum + 1);
    if (0 != db_thread_start(&db_thread_id, &brr_exit, &DBServer))
    {
        perror("db thread start fail!");
        exit(1);
    }
    DBServer.dbStartTime = get_mtime();
    if (0 != update_thread_start(&update_thread_array, &brr_exit,
                                 &DBServer))
    {
        return -3;
    }
    //wait for quit
    pthread_join(db_thread_id, NULL);
    DBServer.dbEndTime = get_mtime();
    for (i = 0; i < DBServer.updateThrNum; i++)
    {
        pthread_join(update_thread_array[i], NULL);
        printf("update thread %d exit!\n", i);
    }
    free(update_thread_array);
    pthread_barrier_destroy(&brr_exit);
    sprintf(logName,"./log/overhead/%d_overhead_%dk_%ld_%d.log",
            DBServer.algType,DBServer.updateFrequency/1000,
            DBServer.dbSize,DBServer.unitSize);
    write_overhead_log(&DBServer,logName);
    //print the database throughput
    FILE *throughputFile;
    char logPath[64];
    sprintf(logPath,"./log/overhead/tps_%d", DBServer.algType);

    throughputFile = fopen(logPath,"w+");
    fprintf(throughputFile,"TPS:%lld",DBServer.update_count / (DBServer.dbEndTime - DBServer.dbStartTime));
    fclose(throughputFile);
    exit(1);
}
