#include"system.h"
#include"pingpong.h"
extern db_server DBServer;

int db_pingpong_init(void *pp_info, size_t db_size)
{
	db_pingpong_infomation *info;

	info = pp_info;
	info->db_size = db_size;
	if (NULL == (info->db_pp_as =numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_pp_as malloc error");
		return -1;
	}
	memset(info->db_pp_as, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_pp_as_odd = numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_pp_as_odd malloc error");
		return -1;
	}
	memset(info->db_pp_as_odd, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_pp_as_even = numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_pp_as_even malloc error");
		return -1;
	}
	memset(info->db_pp_as_even, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_pp_odd_ba = numa_alloc_onnode(db_size , 1))) {
		perror("db_pp_current_odd malloc error");
		return -1;
	}
	memset(info->db_pp_odd_ba, 0, db_size);

	if (NULL == (info->db_pp_even_ba = numa_alloc_onnode(db_size , 1))) {
		perror("db_pp_previous_ba malloc error");
		return -1;
	}
	memset(info->db_pp_even_ba, 1, db_size);

	if (NULL == (info->db_pp_as_previous = numa_alloc_onnode(DBServer.unitSize * db_size , 1))) {
		perror("db_pp_as_previous malloc error");
		return -1;
	}
	memset(info->db_pp_as_even, 'S', DBServer.unitSize * db_size);

    info->db_pp_lock = UNLOCK;
	info->current = 0;
	return 0;
}

void* pingpong_read(size_t index)
{
	if (index > (DBServer.pingpongInfo).db_size)
		index = index % (DBServer.pingpongInfo).db_size;
	return(DBServer.pingpongInfo).db_pp_as + index * DBServer.unitSize;
}

int pingpong_write(size_t index, void* value)
{
	index = index % (DBServer.pingpongInfo).db_size;
		memcpy((DBServer.pingpongInfo).db_pp_as + index * DBServer.unitSize, value, 4);
		if (0 == (DBServer.pingpongInfo).current) {
			memcpy((DBServer.pingpongInfo).db_pp_as_odd + index * DBServer.unitSize + index % DBServer.unitSize, value, 4);
			(DBServer.pingpongInfo).db_pp_odd_ba[index] = 1;
		} else {
			memcpy((DBServer.pingpongInfo).db_pp_as_even + index * DBServer.unitSize + index % DBServer.unitSize, value, 4);
			(DBServer.pingpongInfo).db_pp_even_ba[index] = 1;
		}
	return 0;
}

void db_pingpong_ckp(int ckp_order, void *pp_info)
{
	int ckp_fd;
	char ckp_name[32];
	size_t i;
	size_t db_size;
	db_pingpong_infomation *info;
	char *currentBackup;
	unsigned char *currentBA;
	long long timeStart;
	long long timeEnd;

	info = pp_info;
	sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);
	if (-1 == (ckp_fd = open(ckp_name, O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, 666))) {
		perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
		return;
	}
	db_size = info->db_size;

	timeStart = get_utime();
	//prepare for checkpoint
    pthread_spin_lock(&(DBServer.presync));
	info->current = !(info->current);

	if (0 == info->current) {
		currentBackup = info->db_pp_as_odd;
		currentBA = info->db_pp_odd_ba;
	} else {
		currentBackup = info->db_pp_as_even;
		currentBA = info->db_pp_even_ba;
	}
	pthread_spin_unlock(&(DBServer.presync));
	timeEnd = get_utime();
	add_prepare_log(&DBServer,timeEnd - timeStart);
	timeStart = get_utime();
	for (i = 0; i < db_size; i++) {
		if (1 == currentBA[i]) {
			//info->db_pp_as_previous[i] = info->db_pp_as_even[i];
			memcpy(info->db_pp_as_previous + (size_t)i * DBServer.unitSize,
				currentBackup + (size_t)i * DBServer.unitSize, (size_t)DBServer.unitSize);
			memset(currentBackup + (size_t)i * DBServer.unitSize, 0, (size_t)DBServer.unitSize);
			currentBA[i] = 0;
		}
	}
	writeLarge(ckp_fd, info->db_pp_as_previous, (size_t)DBServer.unitSize * db_size, (size_t)DBServer.unitSize);
	fsync(ckp_fd);
	close(ckp_fd);
	timeEnd = get_utime();
	add_overhead_log(&DBServer,timeEnd - timeStart);
}

void db_pingpong_destroy(void *pp_info)
{
	db_pingpong_infomation *info;

	info = pp_info;
	numa_free(info->db_pp_as , DBServer.unitSize * info->db_size);
	numa_free(info->db_pp_as_even , DBServer.unitSize * info->db_size);
	numa_free(info->db_pp_as_odd , DBServer.unitSize * info->db_size);
	numa_free(info->db_pp_as_previous , info->db_size);
	numa_free(info->db_pp_even_ba , info->db_size);
	numa_free(info->db_pp_odd_ba , info->db_size);

}
