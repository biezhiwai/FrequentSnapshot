#include"src/system/system.h"
#include"zigzag.h"

extern db_server DBServer;

int db_zigzag_init(void *zigzag_info, size_t db_size) {
    db_zigzag_infomation *info;

    info = zigzag_info;

    info->db_size = db_size;

    if (NULL == (info->db_zigzag_as0 = (char *) malloc(DBServer.unitSize * db_size))) {
        perror("db_zigzag_as0 malloc error");
        return -1;
    }
    memset(info->db_zigzag_as0, 'S', DBServer.unitSize * db_size);

    if (NULL == (info->db_zigzag_as1 = (char *) malloc(DBServer.unitSize * db_size))) {
        perror("db_zigzag_sa1 malloc error");
        return -1;
    }
    memset(info->db_zigzag_as0, 'S', DBServer.unitSize * db_size);

    if (NULL == (info->db_zigzag_mr = (bool *) malloc(db_size))) {
        perror("db_zigzag_mr malloc error");
        return -1;
    }
    memset(info->db_zigzag_mr, 0, db_size);

    if (NULL == (info->db_zigzag_mw = (bool *) malloc(db_size))) {
        perror("db_zigzag_mw malloc error");
        return -1;
    }
    memset(info->db_zigzag_mw, 1, db_size);
    return 0;
}

void db_zigzag_destroy(void *zigzag_info) {
    db_zigzag_infomation *info;

    info = zigzag_info;
    free(info->db_zigzag_as0);
    free(info->db_zigzag_as1);
    free(info->db_zigzag_mr);
    free(info->db_zigzag_mw);
}

void *zigzag_read(size_t index) {
//    if (index > (DBServer.zigzagInfo).db_size)
//        index = index % (DBServer.zigzagInfo).db_size;
    if (0 == (DBServer.zigzagInfo).db_zigzag_mr[index]) {
        return (void *) ((DBServer.zigzagInfo).db_zigzag_as0 + index * DBServer.unitSize);
    } else {
        return (void *) ((DBServer.zigzagInfo).db_zigzag_as1 + index * DBServer.unitSize);
    }
}

int zigzag_write(size_t index, void *value) {
    //index = index % (DBServer.zigzagInfo).db_size;
    bool flag = (DBServer.zigzagInfo).db_zigzag_mw[index];
    if (0==flag) {
        memcpy((DBServer.zigzagInfo).db_zigzag_as0 + index * DBServer.unitSize, value, ITEM_SIZE);
        (DBServer.zigzagInfo).db_zigzag_mr[index] = 0;
        return 0;
    } else {
        memcpy((DBServer.zigzagInfo).db_zigzag_as1 + index * DBServer.unitSize, value, ITEM_SIZE);
        (DBServer.zigzagInfo).db_zigzag_mr[index] = 1;
        return 0;
    }
}

void db_zigzag_ckp(int ckp_order, void *zigzag_info) {
    FILE *ckp_fd;
    char ckp_name[128];
    size_t i;
    size_t db_size;
    db_zigzag_infomation *info;
    long long timeStart;
    long long timeEnd;

    info = zigzag_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);
    if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
        perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
        return;
    }
    char* buf = (char*)malloc(1024L*1024*1024);
    setvbuf(ckp_fd,buf,_IOFBF,1024L*1024*1024);
    db_size = info->db_size;

    pthread_spin_lock(&(DBServer.presync));
    //db_lock(&(DBServer.pre_lock));

    timeStart = get_ntime();
    for (i = 0; i < db_size; i++) {
        info->db_zigzag_mw[i] = !(info->db_zigzag_mr[i]);
    }
    timeEnd = get_ntime();

    pthread_spin_unlock(&(DBServer.presync));
    //db_unlock(&(DBServer.pre_lock));

    add_prepare_log(&DBServer, timeEnd - timeStart);
    //write to disk
    timeStart = get_ntime();
    for (i = 0; i < db_size; i++) {
        if (0 == info->db_zigzag_mw[i]) {
            fwrite(info->db_zigzag_as1 + (size_t) i * DBServer.unitSize, (size_t) DBServer.unitSize, 1, ckp_fd);
        } else {
            fwrite(info->db_zigzag_as0 + (size_t) i * DBServer.unitSize, (size_t) DBServer.unitSize, 1, ckp_fd);
        }
    }
    fflush(ckp_fd);
    fclose(ckp_fd);
    timeEnd = get_ntime();
    add_overhead_log(&DBServer, timeEnd - timeStart);
    free(buf);
}


