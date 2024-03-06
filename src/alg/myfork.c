#include "src/include/myfork.h"
#include "src/include/system.h"

extern db_server DBServer;

int db_myfork_init(void *myfork_info, size_t db_size) {
    db_myfork_infomation *info;

    info = myfork_info;
    info->db_size = db_size;

    if (NULL == (info->db_myfork_AS =
                         (char *) malloc(DBServer.rowSize * db_size))) {
        perror("da_navie_AS malloc error");
        return -1;
    }
    memset(info->db_myfork_AS, 'S', DBServer.rowSize * db_size);

    return 0;
}

void db_myfork_destroy(void *myfork_info) {
    db_myfork_infomation *info;
    info = myfork_info;
    free(info->db_myfork_AS);
}

void *myfork_read(size_t index) {
    void *result;
    result = (void *) ((DBServer.myforkInfo).db_myfork_AS + index * DBServer.rowSize);
    return result;
}

int myfork_write(size_t page_index, void *value) {
    integer offset = page_index << DBServer.logscale_pagesize;
    memcpy((DBServer.myforkInfo).db_myfork_AS + offset, value, DBServer.rowSize);
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

    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    if (0 != fork()) {
        timeEnd = get_ntime();
        db_unlock(&(DBServer.pre_lock));
        add_prepare_log(&DBServer, timeEnd - timeStart);
        integer time1 = get_mtime();
        wait(NULL); // waiting for child process exit
        timeEnd = get_mtime();
        add_overhead_log(&DBServer, timeEnd - time1);

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

        fwrite(info->db_myfork_AS, (size_t) db_size * DBServer.rowSize, 1, ckp_fd);

        fflush(ckp_fd);
        fclose(ckp_fd);
        _exit(0);
    }
}
