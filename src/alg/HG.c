#include "src/system/system.h"
#include "HG.h"

extern db_server DBServer;

int db_hg_init(void *ll_info, size_t db_size) {
    db_hg_infomation *info = ll_info;

    info->db_size = db_size;

    if (NULL == (info->db_hg_as0 = malloc(DBServer.pageSize * db_size))) {
        perror("db_hg_as0 malloc error");
        return -1;
    }
    memset(info->db_hg_as0, 'S', DBServer.pageSize * db_size);

    if (NULL == (info->db_hg_as1 = malloc(DBServer.pageSize * db_size))) {
        perror("db_hg_as1 malloc error");
        return -1;
    }
    memset(info->db_hg_as1, 'S', DBServer.pageSize * db_size);
    if (NULL == (info->db_hg_prev = malloc(DBServer.pageSize * db_size))) {
        perror("db_hg_prev malloc error");
        return -1;
    }
    memset(info->db_hg_prev, 'S', DBServer.pageSize * db_size);
    if (NULL == (info->db_hg_as0_ba = (unsigned char *) malloc(db_size))) {
        perror("db_hg_as0_ba malloc error");
        return -1;
    }
    memset(info->db_hg_as0_ba, 0, db_size);
    if (NULL == (info->db_hg_as1_ba = (unsigned char *) malloc(db_size))) {
        perror("db_hg_as1_ba malloc error");
        return -1;
    }
    memset(info->db_hg_as1_ba, 0, db_size);
    if (NULL == (info->db_hg_mr_ba = (unsigned char *) malloc(db_size))) {
        perror("db_hg_as1_ba malloc error");
        return -1;
    }
    memset(info->db_hg_mr_ba, 0, db_size);
    info->current = 0;
    return 0;

}

void db_hg_destroy(void *ll_info) {
    db_hg_infomation *info = ll_info;
    free(info->db_hg_as1);
    free(info->db_hg_as0);
    free(info->db_hg_prev);
    free(info->db_hg_as1_ba);
    free(info->db_hg_as0_ba);
    free(info->db_hg_mr_ba);
}

void *hg_read(size_t index) {
//    if (index > (DBServer.llInfo).db_size)
//        index = index % (DBServer.llInfo).db_size;
    if (1 == (DBServer.hgInfo).current) {
        return (DBServer.hgInfo).db_hg_as1 + index * DBServer.pageSize;
    } else {
        return (DBServer.hgInfo).db_hg_as0 + index * DBServer.pageSize;
    }
}

int hg_write(size_t index_page, void *value) {
    integer offset = index_page << DBServer.logscale_pagesize;
    if (1 == (DBServer.hgInfo).current) {
        memcpy((DBServer.hgInfo).db_hg_as1 + offset, value, FILED_SIZE);
        (DBServer.hgInfo).db_hg_as1_ba[index_page] = 1;
    } else {
        memcpy((DBServer.hgInfo).db_hg_as0 + offset, value, FILED_SIZE);
        (DBServer.hgInfo).db_hg_as0_ba[index_page] = 1;
    }
    return 0;
}

void db_hg_ckp(int ckp_order, void *ll_info) {
    FILE *ckp_fd;
    char ckp_name[32];
    integer i;
    int db_size;
    db_hg_infomation *info;
    char *currentBackup;
    unsigned char *currentBA;
    integer timeStart;
    integer timeEnd;

    info = ll_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    db_size = info->db_size;

    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    //prepare for checkpoint
    info->current = !(info->current);
    if (0 == info->current) {
        currentBackup = info->db_hg_as1;
        currentBA = info->db_hg_as1_ba;
    } else {
        currentBackup = info->db_hg_as0;
        currentBA = info->db_hg_as0_ba;
    }
    timeEnd = get_ntime();
    db_unlock(&(DBServer.pre_lock));
    add_prepare_log(&DBServer, timeEnd - timeStart);
    for (i = 0; i < db_size; i++) {
        if (1 == currentBA[i]) {
            //info->db_pp_as_previous[i] = info->db_pp_as_even[i];
            memcpy(info->db_hg_prev + i * DBServer.pageSize,
                   currentBackup + i * DBServer.pageSize, DBServer.pageSize);
            currentBA[i] = 0;
        }
    }
    timeStart = get_mtime();

    if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
        perror("checkpoint file open error,checkout if the ckp_backup directory "
               "is exist");
        return;
    }
    // char* buf = (char*)malloc(1024L*1024*1024);
    // setvbuf(ckp_fd,buf,_IOFBF,1024L*1024*1024);
    setbuf(ckp_fd, NULL);

    //for (int i = 0; i < db_size; ++i) {
    fwrite(info->db_hg_prev, (size_t) DBServer.pageSize * db_size, 1, ckp_fd);
    //}
    fflush(ckp_fd);
    fclose(ckp_fd);
    timeEnd = get_mtime();
    add_overhead_log(&DBServer, timeEnd - timeStart);

    //free(buf);
}


