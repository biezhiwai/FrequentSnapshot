#include"src/system/system.h"
#include"pingpong.h"

extern db_server DBServer;

int db_pingpong_init(void *pp_info, size_t db_size) {
    db_pingpong_infomation *info;

    info = pp_info;
    info->db_size = db_size;
    if (NULL == (info->db_pp_as = malloc(DBServer.pageSize * db_size))) {
        perror("db_pp_as malloc error");
        return -1;
    }
    memset(info->db_pp_as, 'S', DBServer.pageSize * db_size);

    if (NULL == (info->db_pp_as_odd = malloc(DBServer.pageSize * db_size))) {
        perror("db_pp_as_odd malloc error");
        return -1;
    }
    memset(info->db_pp_as_odd, 'S', DBServer.pageSize * db_size);

    if (NULL == (info->db_pp_as_even = malloc(DBServer.pageSize * db_size))) {
        perror("db_pp_as_even malloc error");
        return -1;
    }
    memset(info->db_pp_as_even, 'S', DBServer.pageSize * db_size);

    if (NULL == (info->db_pp_odd_ba = (unsigned char *) malloc(db_size))) {
        perror("db_pp_current_odd malloc error");
        return -1;
    }
    memset(info->db_pp_odd_ba, 0, db_size);

    if (NULL == (info->db_pp_even_ba = (unsigned char *) malloc(db_size))) {
        perror("db_pp_previous_ba malloc error");
        return -1;
    }
    memset(info->db_pp_even_ba, 1, db_size);

    if (NULL == (info->db_pp_as_previous = malloc(DBServer.pageSize * db_size))) {
        perror("db_pp_as_previous malloc error");
        return -1;
    }
    memset(info->db_pp_as_even, 'S', DBServer.pageSize * db_size);

    info->current = 0;
    return 0;
}

void db_pingpong_destroy(void *pp_info) {
    db_pingpong_infomation *info;

    info = pp_info;
    free(info->db_pp_as);
    free(info->db_pp_as_even);
    free(info->db_pp_as_odd);
    free(info->db_pp_as_previous);
    free(info->db_pp_even_ba);
    free(info->db_pp_odd_ba);
}

void *pingpong_read(size_t index) {
//    if (index > (DBServer.pingpongInfo).db_size)
//        index = index % (DBServer.pingpongInfo).db_size;
    return (DBServer.pingpongInfo).db_pp_as + index * DBServer.pageSize;
}

int pingpong_write(size_t index_page, void *value) {
    integer offset = index_page << DBServer.logscale_pagesize;
    memcpy((DBServer.pingpongInfo).db_pp_as + offset, value, FILED_SIZE);
    if (0 == (DBServer.pingpongInfo).current) {
        memcpy((DBServer.pingpongInfo).db_pp_as_odd + offset, value, FILED_SIZE);
        (DBServer.pingpongInfo).db_pp_odd_ba[index_page] = 1;
    } else {
        memcpy((DBServer.pingpongInfo).db_pp_as_even + offset, value, FILED_SIZE);
        (DBServer.pingpongInfo).db_pp_even_ba[index_page] = 1;
    }
    return 0;
}

void db_pingpong_ckp(int ckp_order, void *pp_info) {
    FILE *ckp_fd;
    char ckp_name[32];
    size_t i;
    size_t db_size;
    db_pingpong_infomation *info;
    char *currentBackup;
    unsigned char *currentBA;
    integer timeStart;
    integer timeEnd;

    info = pp_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    db_size = info->db_size;
    //prepare for checkpoint

    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    info->current = !(info->current);
    if (0 == info->current) {
        currentBackup = info->db_pp_as_odd;
        currentBA = info->db_pp_odd_ba;
    } else {
        currentBackup = info->db_pp_as_even;
        currentBA = info->db_pp_even_ba;
    }
    timeEnd = get_ntime();
    db_unlock(&(DBServer.pre_lock));
    add_prepare_log(&DBServer, timeEnd - timeStart);

    timeStart = get_mtime();

    if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
        perror("checkpoint file open error,checkout if the ckp_backup directory "
               "is exist");
        return;
    }
    // char* buf = (char*)malloc(1024L*1024*1024);
    // setvbuf(ckp_fd,buf,_IOFBF,1024L*1024*1024);
    setbuf(ckp_fd, NULL);
    //char* mem = (char *) malloc(DBServer.pageSize * db_size);
    for (i = 0; i < db_size; i++) {
        if (1 == currentBA[i]) {
            //info->db_pp_as_previous[i] = info->db_pp_as_even[i];
            memcpy(info->db_pp_as_previous + (size_t) i * DBServer.pageSize,
                   currentBackup + (size_t) i * DBServer.pageSize, (size_t) DBServer.pageSize);
            memset(currentBackup + (size_t) i * DBServer.pageSize, 0, (size_t) DBServer.pageSize);
            currentBA[i] = 0;
        }
    }
    //for (int i = 0; i < db_size; ++i) {
    fwrite(info->db_pp_as_previous, (size_t) DBServer.pageSize * db_size, 1, ckp_fd);
    //}
    //fwrite(mem, (size_t) (DBServer.pageSize)*db_size, 1, ckp_fd);
    //free(mem);
    fflush(ckp_fd);
    fclose(ckp_fd);
    timeEnd = get_mtime();
    add_overhead_log(&DBServer, timeEnd - timeStart);

}


