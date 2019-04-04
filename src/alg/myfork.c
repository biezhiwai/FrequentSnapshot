#include "myfork.h"
#include "src/system/system.h"

extern db_server DBServer;

int db_myfork_init(void *myfork_info, size_t db_size) {
    db_myfork_infomation *info;

    info = myfork_info;
    info->db_size = db_size;

    if (NULL == (info->db_myfork_AS =
                         (char *) malloc(DBServer.unitSize * db_size))) {
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

    free(info->db_myfork_AS);
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
    int db_size;

    info = myfork_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    long long time2= get_ntime();
    pthread_spin_lock(&(DBServer.presync));
    timeStart = get_ntime();
    if (0 == fork()) {  // a child checkpoint process
        if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
            perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
            return;
        }
        //char* buf = (char*)malloc(1024L*1024*1024);
        //setvbuf(ckp_fd,buf,_IOFBF,1024L*1024*1024);
        setbuf(ckp_fd,NULL);
        db_size = info->db_size;
        long long time1 = get_ntime();
        for (int i = 0; i < db_size; ++i) {
            fwrite(info->db_myfork_AS + (size_t) i * DBServer.unitSize, (size_t)(DBServer.unitSize), 1, ckp_fd);
        }
        fflush(ckp_fd);
        add_overhead_log(&DBServer, get_ntime() - time1);
        fclose(ckp_fd);

        _exit(0);
    } else {
        add_prepare_log(&DBServer, get_ntime() - timeStart);
        pthread_spin_unlock(&(DBServer.presync));
        long long time1 = get_ntime();
        wait(NULL);   // waiting for child process exit
        add_overhead_log(&DBServer, get_ntime() - time1);
        while (get_ntime() < time2 + 10000000000);
        //free(buf);
    }
}
