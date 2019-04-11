#include"naive.h"
#include"src/system/system.h"


extern db_server DBServer;

int db_naive_init(void *naive_info, size_t db_size) {
    db_naive_infomation *info;
    info = naive_info;
    info->db_size = db_size;

    if (NULL == (info->db_naive_AS =
                         (char *) malloc(DBServer.pageSize * db_size))) {
        perror("da_navie_AS malloc error");
        return -1;
    }
    memset(info->db_naive_AS, 'S', DBServer.pageSize * db_size);

    if (NULL == (info->db_naive_AS_shandow =
                         (char *) malloc(DBServer.pageSize * db_size))) {
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
    result = (void *) ((DBServer.naiveInfo).db_naive_AS + index * DBServer.pageSize);
    return result;
}

int naive_write(size_t index, void *value) {
    memcpy((DBServer.naiveInfo).db_naive_AS + index, value, ITEM_SIZE);
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
    db_size = info->db_size;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    long long time1 = get_mtime();
    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    memcpy(info->db_naive_AS_shandow, info->db_naive_AS,
           (long long) DBServer.pageSize * DBServer.dbSize);
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
    for (int i = 0; i < db_size; ++i) {
        fwrite(info->db_naive_AS_shandow + (size_t) i * DBServer.pageSize, (size_t)(DBServer.pageSize), 1, ckp_fd);
    }
    fflush(ckp_fd);
    fclose(ckp_fd);    // is time consuming
    timeEnd = get_mtime();
    add_overhead_log(&DBServer, timeEnd - timeStart);
    while (get_mtime() < time1 + 10000); // wait 10s
}
