#include"src/include/naive.h"
#include"src/include/system.h"


extern db_server DBServer;

int db_naive_init(void *naive_info, size_t db_size) {
    db_naive_infomation *info;
    info = naive_info;
    info->db_size = db_size;

    if (NULL == (info->db_naive_AS =
                         (char *) malloc(DBServer.rowSize * db_size))) {
        perror("da_navie_AS malloc error");
        return -1;
    }
    memset(info->db_naive_AS, 'S', DBServer.rowSize * db_size);

    if (NULL == (info->db_naive_AS_shandow =
                         (char *) malloc(DBServer.rowSize * db_size))) {
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

void *naive_read(size_t page_index) {
    void *result;
    result = (void *) ((DBServer.naiveInfo).db_naive_AS + (page_index << DBServer.logscale_pagesize));
    return result;
}

int naive_write(size_t page_index, void *value) {
    integer offset = page_index << DBServer.logscale_pagesize;
    memcpy((DBServer.naiveInfo).db_naive_AS + offset, value, FILED_SIZE);
    return 0;
}


void ckp_naive(int ckp_order, void *naive_info) {
    FILE *ckp_fd;
    char ckp_name[32];
    db_naive_infomation *info;
    integer timeStart;
    integer timeEnd;
    integer db_size;

    info = naive_info;
    db_size = info->db_size;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    memcpy(info->db_naive_AS_shandow, info->db_naive_AS,
           (integer) DBServer.rowSize * DBServer.dbSize);
    timeEnd = get_ntime();
    db_unlock(&(DBServer.pre_lock));
    add_prepare_log(&DBServer, timeEnd - timeStart);
    timeStart = get_mtime();
    if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
        perror("checkpoint file open error,checkout if the ckp_backup directory "
               "is exist");
        return;
    }
    setbuf(ckp_fd, NULL);

    fwrite(info->db_naive_AS_shandow, (size_t) db_size * DBServer.rowSize, 1, ckp_fd);

    fflush(ckp_fd);
    fclose(ckp_fd);    // is time consuming
    timeEnd = get_mtime();
    add_overhead_log(&DBServer, timeEnd - timeStart);

}
