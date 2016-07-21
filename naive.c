#include"naive.h"
#include"system.h"

extern db_server DBServer;

int db_naive_init(void *naive_info, size_t db_size)
{
	db_naive_infomation *info;

	info = naive_info;
	info->db_size = db_size;

	if (NULL == (info->db_naive_AS =
		(char *) numa_alloc_onnode(DBServer.unitSize * db_size, 0) )) {
		perror("da_navie_AS malloc error");
		return -1;
	}
	memset(info->db_naive_AS, 'S', DBServer.unitSize * db_size);

	if (NULL == (info->db_naive_AS_shandow =
		(char *) numa_alloc_onnode(DBServer.unitSize * db_size , 0))) {
		perror("db_navie_AS_shandow malloc error");
		return -1;
	}

    //info->db_naive_lock = UNLOCK;
	return 0;
}

void db_naive_destroy(void *naive_info)
{
	db_naive_infomation *info;
	info = naive_info;

	numa_free(info->db_naive_AS , DBServer.unitSize * info->db_size);
	numa_free(info->db_naive_AS_shandow,DBServer.unitSize * info->db_size);
}

void* naive_read(size_t index)
{
	void *result;
	if (index >= DBServer.dbSize) {
		index = index % DBServer.dbSize;
	}
	result = (void *) ((DBServer.naiveInfo).db_naive_AS + index * DBServer.unitSize);
	return result;
}

int naive_write(size_t index, void *value)
{
	index = index % DBServer.dbSize;
		memcpy((DBServer.naiveInfo).db_naive_AS + index * DBServer.unitSize, value, 4);
	return 0;
}

void ckp_naive(int ckp_order, void *naive_info)
{
	int ckp_fd;
	char ckp_name[32];
	db_naive_infomation *info;
	long long timeStart;
	long long timeEnd;
	int db_size;

	info = naive_info;
	sprintf(ckp_name, "./ckp_backup/naive_%d", ckp_order);
	if (-1 == (ckp_fd = open(ckp_name, O_WRONLY | O_CREAT, 666))) {
		perror("checkpoint file open error,checkout if the ckp_backup directory is exist");
		return;
	}
	db_size = info->db_size;
	timeStart = get_utime();

    pthread_spin_lock(&(DBServer.presync));
	memcpy(info->db_naive_AS_shandow,info->db_naive_AS , DBServer.unitSize * db_size);
	pthread_spin_unlock(&(DBServer.presync));

	timeEnd = get_utime();
	add_prepare_log(&DBServer,timeEnd - timeStart);

	timeStart = get_utime();
	write(ckp_fd, info->db_naive_AS_shandow, DBServer.unitSize * db_size);
	fsync(ckp_fd);
	close(ckp_fd);
	timeEnd = get_utime();
	add_overhead_log(&DBServer,timeEnd - timeStart);

}
