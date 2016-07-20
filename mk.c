#include"system.h"
#include"mk.h"
extern db_server DBServer;

void db_mk_lock(int index)
{
    unsigned char expected = 0;

    while(!__atomic_compare_exchange_1(DBServer.mkInfo.db_mk_access + index,&expected,
                                1,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST)){
        expected = 0;
    }
}
void db_mk_unlock(int index)
{
    __atomic_store_n(DBServer.mkInfo.db_mk_access+index,0,__ATOMIC_SEQ_CST);
}
int db_mk_init(void *mk_info, size_t db_size)
{
	db_mk_infomation *info = mk_info;

	info->db_size = db_size;

	if (NULL == (info->db_mk_as1 = numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_mk_as1 malloc error");
		return -1;
	}
	memset(info->db_mk_as1, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_mk_as2 = numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_mk_as2 malloc error");
		return -1;
	}
	memset(info->db_mk_as2, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_mk_ba = numa_alloc_onnode(db_size , 1))) {
		perror("db_mk_ba malloc error");
		return -1;
	}
	memset(info->db_mk_ba, 0, db_size);

    info->db_mk_access = numa_alloc_onnode(db_size , 1);
    memset(info->db_mk_access,0,db_size);
    info->db_mk_lock = UNLOCK;
	info->current = 1;
	return 0;

}

void* mk_read(size_t index)
{
	if (index > (DBServer.mkInfo).db_size)
		index = index % (DBServer.mkInfo).db_size;
	if (1 == (DBServer.mkInfo).current) {
		return(DBServer.mkInfo).db_mk_as1 + index * DBServer.unitSize;
	} else {
		return(DBServer.mkInfo).db_mk_as2 + index * DBServer.unitSize;
	}
	return NULL;
}

int mk_write(size_t index, void* value)
{
	index = index % (DBServer.mkInfo).db_size;
		if (1 == (DBServer.mkInfo).current) {
			memcpy((DBServer.mkInfo).db_mk_as1 + index * DBServer.unitSize , value, 4);
		(DBServer.mkInfo).db_mk_ba[index] = 1;
		} else {
			memcpy((DBServer.mkInfo).db_mk_as2 + index * DBServer.unitSize , value, 4);
			(DBServer.mkInfo).db_mk_ba[index] = 2;
		}
	return 0;
}
typedef struct {
	int fd;
	char *addr;
	int len;
}mk_disk_info;
void *mk_write_to_disk_thr(void *arg)
{
	mk_disk_info *info = arg;
	long long timeStart;
	long long timeEnd;
	timeStart = get_ntime();
	write(info->fd,info->addr,info->len);
	fsync(info->fd);
	close(info->fd);
	timeEnd = get_ntime();
	add_overhead_log(&DBServer,timeEnd - timeStart);
	return NULL;
}
void db_mk_ckp(int ckp_order, void *mk_info)
{
	int ckp_fd;
	char ckp_name[32];
	int i;
	int db_size;
	db_mk_infomation *info;
	mk_disk_info mkDiskInfo;
	pthread_t mkDiskThrId;
	int mkCur;
	char *backup;
	char *online;
	long long timeStart;
	long long timeEnd;
	info = mk_info;
	sprintf(ckp_name, "./ckp_backup/mk_%d", ckp_order);
	if (-1 == (ckp_fd = open(ckp_name, O_WRONLY | O_CREAT, 666))) {
		perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
		return;
	}
	db_size = info->db_size;

	timeStart = get_utime();

  pthread_spin_lock(&(DBServer.presync));
	if ( info->current == 1)
		info->current = 2;
	else
		info->current = 1;
	//info->current = (1 == (info->current)) ? 2 : 1;
  pthread_spin_unlock(&(DBServer.presync));

    timeEnd = get_utime();
    add_prepare_log( &DBServer, timeEnd - timeStart);

    timeStart = get_utime();
    if (1 == info->current) {
		mkCur = 1;
		online = info->db_mk_as1;
		backup = info->db_mk_as2;

	} else {
		mkCur = 2;
		online = info->db_mk_as2;
		backup = info->db_mk_as1;
	}
    write(ckp_fd,backup,DBServer.dbSize * DBServer.unitSize);
    fsync(ckp_fd);
    close(ckp_fd);
/*	mkDiskInfo.fd = ckp_fd;
	mkDiskInfo.len = DBServer.dbSize * DBServer.unitSize;
	mkDiskInfo.addr = backup;
	pthread_create(&mkDiskThrId,NULL,mk_write_to_disk_thr,&mkDiskInfo);
    */
	for (i = 0; i < db_size; i++) {

		if (mkCur != info->db_mk_ba[i] && 0 != mkCur) {
			memcpy(online + i * DBServer.unitSize,
				backup + i * DBServer.unitSize, DBServer.unitSize);
			info->db_mk_ba[i] = 0;
		}

	}
    timeEnd = get_utime();
    add_overhead_log(&DBServer,timeEnd - timeStart);
}

void db_mk_destroy(void *mk_info)
{
	db_mk_infomation *info = mk_info;
	numa_free(info->db_mk_as1 , DBServer.unitSize * info->db_size);
	numa_free(info->db_mk_as2 , DBServer.unitSize * info->db_size);
	numa_free(info->db_mk_ba , info->db_size);
    numa_free(info->db_mk_access , info->db_size);

}
