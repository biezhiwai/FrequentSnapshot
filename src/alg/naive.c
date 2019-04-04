#include"naive.h"
#include"src/system/system.h"


extern db_server DBServer;

int db_naive_init(void *naive_info, size_t db_size) {
    db_naive_infomation *info;

    info = naive_info;
    info->db_size = db_size;

    if (NULL == (info->db_naive_AS =
                         (char *) malloc(DBServer.unitSize * db_size))) {
        perror("da_navie_AS malloc error");
        return -1;
    }
    memset(info->db_naive_AS, 'S', DBServer.unitSize * db_size);

    if (NULL == (info->db_naive_AS_shandow =
                         (char *) malloc(DBServer.unitSize * db_size))) {
        perror("db_navie_AS_shandow malloc error");
        return -1;
    }
    return 0;
}

void db_naive_destroy(void *naive_info) {
    db_naive_infomation *info;
    info = naive_info;

    free(info->db_naive_AS);
    free(info->db_naive_AS_shandow);
}

void *naive_read(size_t index) {
    void *result;
//    if (index >= DBServer.dbSize) {
//        index = index % DBServer.dbSize;
//    }
    result = (void *) ((DBServer.naiveInfo).db_naive_AS + index * DBServer.unitSize);
    return result;
}

int naive_write(size_t index, void *value) {
    //index = index % DBServer.dbSize;
    memcpy((DBServer.naiveInfo).db_naive_AS + index * DBServer.unitSize, value, ITEM_SIZE);
    return 0;
}

void ckp_naive(int ckp_order, void *naive_info) {
    FILE *ckp_fd;
    //int ckp_fd = 0;
    char ckp_name[32];
    db_naive_infomation *info;
    long long timeStart;
    long long timeEnd;
    long long db_size;

    info = naive_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);
    if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
        perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
        return;
    }
//    if(-1 == (ckp_fd = open64(ckp_name,O_WRONLY | O_TRUNC | O_SYNC | O_CREAT , 600))){
//        perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
//        return;
//    }
    //char* buf = (char*)malloc(1024L*1024*1024);
    //setvbuf(ckp_fd,buf,_IOFBF,1024L*1024*1024);
    setbuf(ckp_fd,NULL);
    db_size = info->db_size;
    long long time1= get_ntime();
    pthread_spin_lock(&(DBServer.presync));
    //db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    memcpy(info->db_naive_AS_shandow, info->db_naive_AS, (size_t) DBServer.unitSize * db_size);
    timeEnd = get_ntime();
    //db_unlock(&(DBServer.pre_lock));
    pthread_spin_unlock(&(DBServer.presync));
    add_prepare_log(&DBServer, timeEnd - timeStart);

    timeStart = get_ntime();
    for (int i = 0; i < db_size; ++i) {
        fwrite(info->db_naive_AS_shandow + (size_t) i * DBServer.unitSize, (size_t)(DBServer.unitSize), 1, ckp_fd);
    }

    //pwrite64(ckp_fd,info->db_naive_AS_shandow,(unsigned long long)(DBServer.unitSize) * db_size,0);
    fflush(ckp_fd);
    //fsync(ckp_fd);
    fclose(ckp_fd);    // is time consuming
    timeEnd = get_ntime();
    add_overhead_log(&DBServer, timeEnd - timeStart);

    //close(ckp_fd);
    while (get_ntime() < time1 + 10000000000);
}
