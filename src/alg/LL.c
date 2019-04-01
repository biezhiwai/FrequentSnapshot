#include "src/system/system.h"
#include "LL.h"

extern db_server DBServer;

int db_ll_init(void *ll_info, size_t db_size) {
    db_ll_infomation *info = ll_info;

    info->db_size = db_size;

    if (NULL == (info->db_ll_as0 = numa_alloc_onnode(DBServer.unitSize * db_size, 1))) {
        perror("db_ll_as0 malloc error");
        return -1;
    }
    memset(info->db_ll_as0, 'S', DBServer.unitSize * db_size);

    if (NULL == (info->db_ll_as1 = numa_alloc_onnode(DBServer.unitSize * db_size, 1))) {
        perror("db_ll_as1 malloc error");
        return -1;
    }
    memset(info->db_ll_as1, 'S', DBServer.unitSize * db_size);
    if (NULL == (info->db_ll_prev = numa_alloc_onnode(DBServer.unitSize * db_size, 1))) {
        perror("db_ll_prev malloc error");
        return -1;
    }
    memset(info->db_ll_prev, 'S', DBServer.unitSize * db_size);
    if (NULL == (info->db_ll_as0_ba = numa_alloc_onnode(db_size, 1))) {
        perror("db_ll_as0_ba malloc error");
        return -1;
    }
    memset(info->db_ll_as0_ba, 0, db_size);
    if (NULL == (info->db_ll_as1_ba = numa_alloc_onnode(db_size, 1))) {
        perror("db_ll_as1_ba malloc error");
        return -1;
    }
    memset(info->db_ll_as1_ba, 0, db_size);
    if (NULL == (info->db_ll_mr_ba = numa_alloc_onnode(db_size, 1))) {
        perror("db_ll_as1_ba malloc error");
        return -1;
    }
    memset(info->db_ll_mr_ba, 0, db_size);
    info->db_ll_lock = UNLOCK;
    info->current = 0;
    return 0;

}

void *ll_read(size_t index) {
//    if (index > (DBServer.llInfo).db_size)
//        index = index % (DBServer.llInfo).db_size;
    if (1 == (DBServer.llInfo).current) {
        return (DBServer.llInfo).db_ll_as1 + index * DBServer.unitSize;
    } else {
        return (DBServer.llInfo).db_ll_as0 + index * DBServer.unitSize;
    }
    return NULL;
}

int ll_write(size_t index, void *value) {
    //index = index % (DBServer.llInfo).db_size;
    if (1 == (DBServer.llInfo).current) {
        memcpy((DBServer.llInfo).db_ll_as1 + index * DBServer.unitSize, value, sizeof(size_t) * 4);
        (DBServer.llInfo).db_ll_as1_ba[index] = 1;
    } else {
        memcpy((DBServer.llInfo).db_ll_as0 + index * DBServer.unitSize, value, sizeof(size_t) * 4);
        (DBServer.llInfo).db_ll_as0_ba[index] = 1;
    }
    return 0;
}

void db_ll_ckp(int ckp_order, void *ll_info) {
    FILE *ckp_fd;
    char ckp_name[32];
    int i;
    int db_size;
    db_ll_infomation *info;
    char *currentBackup;
    unsigned char *currentBA;
    long long timeStart;
    long long timeEnd;

    info = ll_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);
    if (NULL == (ckp_fd = fopen(ckp_name, "w+"))) {
        perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
        return;
    }
    db_size = info->db_size;

    //pthread_spin_lock( &(DBServer.presync) );
    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    //prepare for checkpoint
    info->current = !(info->current);
    if (0 == info->current) {
        currentBackup = info->db_ll_as1;
        currentBA = info->db_ll_as1_ba;
    } else {
        currentBackup = info->db_ll_as0;
        currentBA = info->db_ll_as0_ba;
    }
    timeEnd = get_ntime();
    //pthread_spin_unlock( &(DBServer.presync) );
    db_unlock(&(DBServer.pre_lock));
    add_prepare_log(&DBServer, timeEnd - timeStart);
    for (i = 0; i < db_size; i++) {
        if (1 == currentBA[i]) {
            //info->db_pp_as_previous[i] = info->db_pp_as_even[i];
            memcpy(info->db_ll_prev + i * DBServer.unitSize,
                   currentBackup + i * DBServer.unitSize, DBServer.unitSize);
            currentBA[i] = 0;
        }
    }
    timeStart = get_ntime();
    fwrite(info->db_ll_prev, DBServer.unitSize, db_size, ckp_fd);
    fflush(ckp_fd);
    fclose(ckp_fd);
    timeEnd = get_ntime();
    add_overhead_log(&DBServer, timeEnd - timeStart);
}

void db_ll_destroy(void *ll_info) {
    db_ll_infomation *info = ll_info;
    numa_free(info->db_ll_as1, DBServer.unitSize * info->db_size);
    numa_free(info->db_ll_as0, DBServer.unitSize * info->db_size);
    numa_free(info->db_ll_prev, DBServer.unitSize * info->db_size);
    numa_free(info->db_ll_as1_ba, info->db_size);
    numa_free(info->db_ll_as0_ba, info->db_size);
    numa_free(info->db_ll_mr_ba, info->db_size);

}
