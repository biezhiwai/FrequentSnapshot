#include"system.h"
#include"cou.h"
extern db_server DBServer;

void db_cou_lock(int index)
{
    unsigned char expected = 0;

    while(!__atomic_compare_exchange_1(DBServer.couInfo.db_cou_access + index,&expected,
                                1,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST)){
        expected = 0;
    }
}
void db_cou_unlock(int index)
{
    __atomic_store_n(DBServer.couInfo.db_cou_access+index,0,__ATOMIC_SEQ_CST);
}
int db_cou_init(void *cou_info, size_t db_size)
{
	db_cou_infomation *info;

	info = cou_info;
	info->db_size = db_size;

	if (NULL == (info->db_cou_primary =
		(char *) numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_cou_primary malloc error");
		return -1;
	}
	memset(info->db_cou_primary, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_cou_shandow =
		(char *) numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_cou_shandow malloc error");
		return -1;
	}
	memset(info->db_cou_shandow, 'S', DBServer.unitSize * db_size);

    if (NULL == (info->db_cou_curBA =
		(unsigned char *) numa_alloc_onnode(db_size , 1))) {
		perror("db_cou_bitarray malloc error");
		return -1;
	}

    if (NULL == (info->db_cou_chgBA =
        (unsigned char *) numa_alloc_onnode(db_size , 1))) {
        perror("db_cou_bitarray malloc error");
        return -1;
    }

    if (NULL == (info->db_cou_preBA =
        (unsigned char *) numa_alloc_onnode(db_size , 1))) {
        perror("db_cou_bitarray malloc error");
        return -1;
    }
    memset(info->db_cou_curBA, 0, db_size);
    memset(info->db_cou_preBA, 0, db_size);
    memset(info->db_cou_chgBA, 0, db_size);
    info->db_cou_access = numa_alloc_onnode(db_size , 1);
    memset(info->db_cou_access,0,db_size);
    info->db_cou_lock = UNLOCK;
	return 0;
}

void* cou_read(size_t index)
{
	void *result;
	if (index > DBServer.dbSize)
		index = index % DBServer.dbSize;
	result = (DBServer.couInfo).db_cou_primary + index * DBServer.unitSize;
	return result;
}

int cou_write(size_t index, void *value)
{
	index = index % DBServer.dbSize;
    	db_lock( &(DBServer.couInfo.db_cou_lock));
    	if ( !DBServer.couInfo.db_cou_curBA[index]){
        	db_cou_lock(index);
        	if ( DBServer.couInfo.db_cou_chgBA[index])
            	memcpy(DBServer.couInfo.db_cou_shandow + index * DBServer.unitSize,value,4);
        	DBServer.couInfo.db_cou_curBA[index] = 1;
        	db_cou_unlock(index);
    	}
    	memcpy(DBServer.couInfo.db_cou_primary + index * DBServer.unitSize,value,4);
    	db_unlock( &(DBServer.couInfo.db_cou_lock));
    return 0;
}

void ckp_cou(int ckp_order, void *cou_info)
{
	int ckp_fd;
	char ckp_name[32];
	size_t i;
	size_t db_size;
	db_cou_infomation *info;
	long long timeStart;
	long long timeEnd;
    static int times = 0;
	info = cou_info;
	sprintf(ckp_name, "./ckp_backup/cou_%d", ckp_order);
    if (-1 == (ckp_fd = open(ckp_name, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, 666))) {
		perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
		return;
	}
	db_size = info->db_size;
	timeStart = get_utime();
	pthread_spin_lock( &(DBServer.presync) );
    db_lock( &(DBServer.couInfo.db_cou_lock));
    for (i = 0; i < db_size; i++) {
        info->db_cou_chgBA[i] = info->db_cou_curBA[i] | info->db_cou_preBA[i];
        info->db_cou_preBA[i] = info->db_cou_curBA[i];
        info->db_cou_curBA[i] = 1;
	}
    db_unlock( &(DBServer.couInfo.db_cou_lock));
    pthread_spin_unlock( &(DBServer.presync) );
	timeEnd = get_utime();
	add_prepare_log(&DBServer,timeEnd - timeStart);

	timeStart = get_utime();
    if ( !times){
        writeLarge(ckp_fd, info->db_cou_shandow, (size_t)DBServer.unitSize * db_size , (size_t)DBServer.unitSize);
        times++;
    }else{
        for (i = 0;i < db_size; i ++){
            if (info->db_cou_chgBA[i]){
                db_cou_lock(i);
                if (info->db_cou_curBA[i]){
                    write(ckp_fd, info->db_cou_shandow + (size_t)i * DBServer.unitSize,(size_t)DBServer.unitSize);
					lseek(ckp_fd, 0, SEEK_END);
                    db_cou_unlock(i);
                }else{
                    write(ckp_fd, info->db_cou_primary + (size_t)i * DBServer.unitSize,(size_t)DBServer.unitSize);
					lseek(ckp_fd, 0, SEEK_END);
                    db_cou_unlock(i);
                }
            }
        }
    }

	fsync(ckp_fd);
	close(ckp_fd);
	timeEnd = get_utime();
	add_overhead_log(&DBServer,timeEnd - timeStart);
}

void db_cou_destroy(void *cou_info)
{
	db_cou_infomation *info;
	info = cou_info;
    numa_free(info->db_cou_chgBA , info->db_size);
    numa_free(info->db_cou_curBA , info->db_size);
    numa_free(info->db_cou_preBA , info->db_size);
	numa_free(info->db_cou_shandow , DBServer.unitSize * info->db_size);
	numa_free(info->db_cou_primary , DBServer.unitSize * info->db_size);
    numa_free(info->db_cou_access , info->db_size);
}
