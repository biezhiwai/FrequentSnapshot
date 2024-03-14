#include"src/include/system.h"
#include"src/include/mm.h"

extern db_server DBServer;


int db_mm_init(void *mm_info, size_t db_size) {
    db_mm_infomation *info = mm_info;

    info->db_size = db_size;
    info->to_dump = malloc(sizeof(Stack));
    info->to_dump->base = malloc(sizeof(integer) * db_size);
    info->to_dump->top = info->to_dump->base;
    info->to_dump->vis = malloc(sizeof(bool) * db_size);
    memset(info->to_dump->vis, 0, db_size);

    info->update = malloc(sizeof(Stack));
    info->update->base = malloc(sizeof(integer) * db_size);
    info->update->top = info->update->base;
    info->update->vis = malloc(sizeof(bool) * db_size);
    memset(info->update->vis, 0, db_size);
    
    if (NULL == (info->db_mm_as1 = malloc(DBServer.rowSize * db_size))) {
        perror("db_mm_as1 malloc error");
        return -1;
    }
    memset(info->db_mm_as1, 'S', DBServer.rowSize * db_size);
    return 0;
}

void db_mm_destroy(void *mm_info) {
    db_mm_infomation *info = mm_info;
    free(info->db_mm_as1);
    free(info->to_dump->base);
    free(info->to_dump->vis);
    free(info->to_dump);
    free(info->update->base);
    free(info->update->vis);
    free(info->update);
}

void *mm_read(size_t index) {
    return (DBServer.mmInfo).db_mm_as1 + index * DBServer.rowSize;
}

int mm_write(size_t index_page, void *value) {
    integer index = index_page << DBServer.logscale_pagesize;
    memcpy((DBServer.mmInfo).db_mm_as1 + index, value, FILED_SIZE);
    if(!DBServer.mmInfo.update->vis[index_page]){
        *(DBServer.mmInfo.update->top++) = index_page;
        DBServer.mmInfo.update->vis[index_page] = 1;
    }
    return 0;
}


void db_mm_ckp(int ckp_order, void *mm_info) {
    int ckp_fd = -1;
    char ckp_name[32];
    int db_size;
    db_mm_infomation *info;

    integer timeStart;
    integer timeEnd;
    info = mm_info;
    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    db_size = info->db_size;
    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    Stack* tmp = info->to_dump;
    info->to_dump = info->update;
    info->update = tmp;
    timeEnd = get_ntime();
    db_unlock(&(DBServer.pre_lock));
    add_prepare_log(&DBServer, timeEnd - timeStart);

    timeStart = get_mtime();
    ckp_fd = open(ckp_name, O_CREAT | O_RDWR, 0600);
    if(ckp_fd == -1){
        perror("checkpoint file open error,checkout if the ckp_backup directory "
               "is exist");
        return;
    }
    ftruncate(ckp_fd, DBServer.rowSize * db_size);
    void* db_mm_addr = mmap(NULL, DBServer.rowSize * db_size, PROT_READ | PROT_WRITE, MAP_SHARED, ckp_fd, 0);

    while(info->to_dump->top != info->to_dump->base){
        size_t index_page = *(--info->to_dump->top);
        integer index = index_page << DBServer.logscale_pagesize;
        memcpy(db_mm_addr + index, info->db_mm_as1 + index, DBServer.rowSize);
        info->to_dump->vis[index_page] = 0;
    }

    msync(db_mm_addr, DBServer.rowSize * db_size, MS_SYNC);
    munmap(db_mm_addr, DBServer.rowSize * info->db_size);
    db_mm_addr = NULL;
    close(ckp_fd);
    ckp_fd = -1;

    timeEnd = get_mtime();
    
    add_overhead_log(&DBServer, timeEnd - timeStart);
}


