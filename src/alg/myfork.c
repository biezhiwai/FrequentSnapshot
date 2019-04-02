#include "myfork.h"
#include "src/system/system.h"

extern db_server DBServer;

int db_myfork_init(void *myfork_info, size_t db_size) {
    db_myfork_infomation *info;

    info = myfork_info;
    info->db_size = db_size;

    if (NULL == (info->db_myfork_AS =
                         (char *) numa_alloc_onnode(DBServer.unitSize * db_size, 0))) {
        perror("da_navie_AS malloc error");
        return -1;
    }
    memset(info->db_myfork_AS, 'S', DBServer.unitSize * db_size);
    /*
    if (NULL == (info->db_myfork_AS_shandow =
        (char *) numa_alloc_onnode(DBServer.unitSize * db_size , 0))) {
        perror("db_navie_AS_shandow malloc error");
        return -1;
    }*/
    return 0;
}

void db_myfork_destroy(void *myfork_info) {
    db_myfork_infomation *info;
    info = myfork_info;

    numa_free(info->db_myfork_AS, DBServer.unitSize * info->db_size);
    //numa_free(info->db_myfork_AS_shandow,DBServer.unitSize * info->db_size);
}

void *myfork_read(size_t index) {
    void *result;
//    if (index >= DBServer.dbSize) {
//        index = index % DBServer.dbSize;
//    }
    result = (void *) ((DBServer.myforkInfo).db_myfork_AS + index * DBServer.unitSize);
    return result;
}

int myfork_write(size_t index, void *value) {
    //index = index % DBServer.dbSize;
    memcpy((DBServer.myforkInfo).db_myfork_AS + index * DBServer.unitSize, value, ITEM_SIZE);
    return 0;
}

void ckp_myfork(int ckp_order, void *myfork_info) {
    FILE *ckp_fd;
    char ckp_name[32];
    db_myfork_infomation *info;
    long long timeStart;
    long long timeEnd;
    int db_size;

    info = myfork_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);
    if (NULL == (ckp_fd = fopen(ckp_name, "w+"))) {
        perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
        return;
    }
    db_size = info->db_size;

    pid_t fpid;
    pthread_spin_lock(&(DBServer.presync));
    timeStart = get_ntime();
    fpid = fork();
    timeEnd = get_ntime();
    pthread_spin_unlock(&(DBServer.presync));
    add_prepare_log(&DBServer, timeEnd - timeStart);

    if (0 == fpid) {
        timeStart = get_ntime();
        //write(ckp_fd, info->db_myfork_AS_shandow,(size_t)DBServer.unitSize * db_size);
        //write for large file
#ifndef OFF_DUMP
        //writeLarge(ckp_fd, info->db_myfork_AS, (size_t)DBServer.unitSize * db_size, (size_t)DBServer.unitSize);
        fwrite(info->db_myfork_AS, DBServer.unitSize, db_size, ckp_fd);
#endif
        fflush(ckp_fd);
        fclose(ckp_fd);
        timeEnd = get_ntime();
        add_overhead_log(&DBServer, timeEnd - timeStart);
        _exit(-1);
    }else{
wait(NULL);
}
}
