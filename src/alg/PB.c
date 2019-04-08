#include"src/system/system.h"
#include"PB.h"

extern db_server DBServer;

void db_mk_lock(int index) {
    unsigned char expected = 0;

    while (!__atomic_compare_exchange_1(DBServer.pbInfo.db_pb_access + index, &expected,
                                        1, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
        expected = 0;
    }
}

void db_mk_unlock(int index) {
    __atomic_store_n(DBServer.pbInfo.db_pb_access + index, 0, __ATOMIC_SEQ_CST);
}

int db_pb_init(void *mk_info, size_t db_size) {
    db_pb_infomation *info = mk_info;

    info->db_size = db_size;

    if (NULL == (info->db_pb_as1 = malloc(DBServer.unitSize * db_size))) {
        perror("db_pb_as1 malloc error");
        return -1;
    }
    memset(info->db_pb_as1, 'S', DBServer.unitSize * db_size);

    if (NULL == (info->db_pb_as2 = malloc(DBServer.unitSize * db_size))) {
        perror("db_pb_as2 malloc error");
        return -1;
    }
    memset(info->db_pb_as2, 'S', DBServer.unitSize * db_size);

    if (NULL == (info->db_pb_ba = (unsigned char*)malloc(db_size))) {
        perror("db_pb_ba malloc error");
        return -1;
    }
    memset(info->db_pb_ba, 0, db_size);

    info->db_pb_access = malloc(db_size);
    memset(info->db_pb_access, 0, db_size);
    info->current = 1;
    return 0;

}

void db_pb_destroy(void *mk_info) {
    db_pb_infomation *info = mk_info;
    free(info->db_pb_as1);
    free(info->db_pb_as2);
    free(info->db_pb_ba);
    free(info->db_pb_access);
}

void *pb_read(size_t index) {
//    if (index > (DBServer.mkInfo).db_size)
//        index = index % (DBServer.mkInfo).db_size;
    if (1 == (DBServer.pbInfo).current) {
        return (DBServer.pbInfo).db_pb_as1 + index * DBServer.unitSize;
    } else {
        return (DBServer.pbInfo).db_pb_as2 + index * DBServer.unitSize;
    }
    return NULL;
}

int pb_write(size_t index, void *value) {
    long index_page = index >> 12;
    if (1 == (DBServer.pbInfo).current) {
        memcpy((DBServer.pbInfo).db_pb_as1 + index, value, ITEM_SIZE);
        (DBServer.pbInfo).db_pb_ba[index_page] = 1;
    } else {
        memcpy((DBServer.pbInfo).db_pb_as2 + index, value, ITEM_SIZE);
        (DBServer.pbInfo).db_pb_ba[index_page] = 2;
    }
    return 0;
}

typedef struct {
    int fd;
    char *addr;
    int len;
} mk_disk_info;

void *mk_write_to_disk_thr(void *arg) {
    mk_disk_info *info = arg;
    long long timeStart;
    long long timeEnd;
    timeStart = get_mtime();
    write(info->fd, info->addr, info->len);
    fsync(info->fd);
    close(info->fd);
    timeEnd = get_mtime();
    add_overhead_log(&DBServer, timeEnd - timeStart);
    return NULL;
}

void db_pb_ckp(int ckp_order, void *mk_info) {
    FILE *ckp_fd;
    char ckp_name[32];
    size_t i;
    int db_size;
    db_pb_infomation *info;
    //mk_disk_info mkDiskInfo;
    //pthread_t mkDiskThrId;
    int mkCur;
    char *backup;
    char *online;
    long long timeStart;
    long long timeEnd;
    info = mk_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    db_size = info->db_size;

    long long time1= get_mtime();

    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    if (info->current == 1)
        info->current = 2;
    else
        info->current = 1;
    //info->current = (1 == (info->current)) ? 2 : 1;
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

    if (1 == info->current) {
        mkCur = 1;
        online = info->db_pb_as1;
        backup = info->db_pb_as2;

    } else {
        mkCur = 2;
        online = info->db_pb_as2;
        backup = info->db_pb_as1;
    }

    for (int i = 0; i < db_size; ++i) {
        fwrite(backup + (size_t) i * DBServer.unitSize, (size_t)(DBServer.unitSize), 1, ckp_fd);
    }

    //writeLarge(ckp_fd,backup,(size_t)DBServer.dbSize * DBServer.unitSize, (size_t)DBServer.unitSize);

    fflush(ckp_fd);

    fclose(ckp_fd);
/*	mkDiskInfo.fd = ckp_fd;
	mkDiskInfo.len = DBServer.dbSize * DBServer.unitSize;
	mkDiskInfo.addr = backup;
	pthread_create(&mkDiskThrId,NULL,mk_write_to_disk_thr,&mkDiskInfo);
    */
    for (i = 0; i < db_size; i++) {

        if (mkCur != info->db_pb_ba[i] && 0 != mkCur) {
            memcpy(online + i * DBServer.unitSize,
                   backup + i * DBServer.unitSize, (size_t) DBServer.unitSize);
            info->db_pb_ba[i] = 0;
        }

    }
    timeEnd = get_mtime();
    add_overhead_log(&DBServer, timeEnd - timeStart);
    //free(buf);

    while (get_mtime() < time1 + 10000);
}


