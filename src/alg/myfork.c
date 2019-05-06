#include "myfork.h"
#include "src/system/system.h"

extern db_server DBServer;

int db_myfork_init(void *myfork_info, size_t db_size) {
    db_myfork_infomation *info;

    info = myfork_info;
    info->db_size = db_size;

    if (NULL == (info->db_myfork_AS =
                         (char *) malloc(DBServer.pageSize * db_size))) {
        perror("da_navie_AS malloc error");
        return -1;
    }
    memset(info->db_myfork_AS, 'S', DBServer.pageSize * db_size);

    return 0;
}

void db_myfork_destroy(void *myfork_info) {
    db_myfork_infomation *info;
    info = myfork_info;
    free(info->db_myfork_AS);
}

void *myfork_read(size_t index) {
    void *result;
    result = (void *) ((DBServer.myforkInfo).db_myfork_AS + index * DBServer.pageSize);
    return result;
}

int myfork_write(size_t index, void *value) {
    memcpy((DBServer.myforkInfo).db_myfork_AS + (index << DBServer.logscale_pagesize), value, DBServer.pageSize);
    return 0;
}

void ckp_myfork(int ckp_order, void *myfork_info) {
    FILE *ckp_fd;
    char ckp_name[32];
    db_myfork_infomation *info;
    integer timeStart, timeEnd;
    int db_size;

    info = myfork_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    integer time2= get_mtime();

    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    if (0 != fork()) {
        timeEnd = get_ntime();
        db_unlock(&(DBServer.pre_lock));
        add_prepare_log(&DBServer, timeEnd - timeStart);
        integer time1 = get_mtime();
        wait(NULL); // waiting for child process exit
        add_overhead_log(&DBServer, get_mtime() - time1);
        while (get_mtime() < time2 + 10000);
    } else {  // a child checkpoint process
        if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
            perror("checkpoint file open error,checkout if the ckp_backup "
                   "directory is exist");
            return;
        }
        // char* buf = (char*)malloc(1024L*1024*1024);
        // setvbuf(ckp_fd,buf,_IOFBF,1024L*1024*1024);
        setbuf(ckp_fd, NULL);
        db_size = info->db_size;
        for (int i = 0; i < db_size; ++i) {
            fwrite(info->db_myfork_AS + (size_t) i * DBServer.pageSize,
                   (size_t) (DBServer.pageSize), 1, ckp_fd);
        }
        fflush(ckp_fd);
        fclose(ckp_fd);
        _exit(0);
    }
}
