#include"system.h"
#include"zigzag.h"
extern db_server DBServer;

int db_zigzag_init(void *zigzag_info, size_t db_size)
{
	db_zigzag_infomation *info;

	info = zigzag_info;

	info->db_size = db_size;

	if (NULL == (info->db_zigzag_as0 = (char *) numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_zigzag_as0 malloc error");
		return -1;
	}
	memset(info->db_zigzag_as0, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_zigzag_as1 = (char *) numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_zigzag_sa1 malloc error");
		return -1;
	}
	memset(info->db_zigzag_as0, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_zigzag_mr = (unsigned char *) numa_alloc_onnode(db_size , 1))) {
		perror("db_zigzag_mr malloc error");
		return -1;
	}
	memset(info->db_zigzag_mr, 0, db_size);

	if (NULL == (info->db_zigzag_mw = (unsigned char *) numa_alloc_onnode(db_size , 1))) {
		perror("db_zigzag_mw malloc error");
		return -1;
	}
	memset(info->db_zigzag_mw, 1, db_size);
    info->db_zigzag_lock = UNLOCK;
	return 0;
}

void* zigzag_read(size_t index)
{
	if (index > (DBServer.zigzagInfo).db_size)
		index = index % (DBServer.zigzagInfo).db_size;
	if (0 == (DBServer.zigzagInfo).db_zigzag_mr[index]) {
		return(void *) ((DBServer.zigzagInfo).db_zigzag_as0 + index * DBServer.unitSize);
	} else {
		return(void *) ((DBServer.zigzagInfo).db_zigzag_as1 + index * DBServer.unitSize);
	}
}

int zigzag_write(size_t index, void *value)
{
	index = index % (DBServer.zigzagInfo).db_size;
		if (0 == (DBServer.zigzagInfo).db_zigzag_mw[index]) {
			memcpy((DBServer.zigzagInfo).db_zigzag_as0 + index * DBServer.unitSize , value, 4);
		} else {
			memcpy((DBServer.zigzagInfo).db_zigzag_as1 + index * DBServer.unitSize , value, 4);
		}
		(DBServer.zigzagInfo).db_zigzag_mr[index] = (DBServer.zigzagInfo).db_zigzag_mw[index];
	return 0;
}

void db_zigzag_ckp(int ckp_order, void *zigzag_info)
{
	int ckpfd;
	char ckp_name[128];
	size_t i;
	size_t db_size;
	db_zigzag_infomation *info;
	long long timeStart;
	long long timeEnd;

	info = zigzag_info;
	sprintf(ckp_name, "./ckp_backup/zz_%d", ckp_order);
	if (-1 == (ckpfd = open(ckp_name,O_WRONLY |O_TRUNC | O_SYNC | O_CREAT,666))) {
		perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
		return;
	}
	db_size = info->db_size;
	timeStart = get_utime();

	pthread_spin_lock(&(DBServer.presync));
	for (i = 0; i < db_size; i++) {
		info->db_zigzag_mw[i] = !(info->db_zigzag_mr[i]);
	}
	pthread_spin_unlock(&(DBServer.presync));

	timeEnd = get_utime();
	add_prepare_log(&DBServer,timeEnd - timeStart);
	//write to disk
	timeStart = get_utime();
	for (i = 0; i < db_size; i++) {
		if (0 == info->db_zigzag_mw[i]) {
			write(ckpfd,info->db_zigzag_as1 + (size_t)i * DBServer.unitSize,
				(size_t)DBServer.unitSize);
				lseek(ckpfd,0,SEEK_END);
		} else {
			write(ckpfd,info->db_zigzag_as0 + (size_t)i * DBServer.unitSize,
				(size_t)DBServer.unitSize);
				lseek(ckpfd,0,SEEK_END);
		}
	}
	fsync(ckpfd);
	close(ckpfd);
	timeEnd = get_utime();
	add_overhead_log(&DBServer,timeEnd - timeStart);
}

void db_zigzag_destroy(void *zigzag_info)
{
	db_zigzag_infomation *info;

	info = zigzag_info;
	numa_free(info->db_zigzag_as0 , DBServer.unitSize * info->db_size);
	numa_free(info->db_zigzag_as1 , DBServer.unitSize * info->db_size);
	numa_free(info->db_zigzag_mr , info->db_size);
	numa_free(info->db_zigzag_mw , info->db_size);

}
