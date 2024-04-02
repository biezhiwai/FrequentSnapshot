#include "src/include/system.h"
#include "src/include/myfork_hotcold.h"

extern db_server DBServer;


int db_myfork_hotcold_init(void *myfork_hotcold_info, size_t db_size) {
    db_myfork_hotcold_infomation *info;
    info = myfork_hotcold_info;

    info->db_size = db_size;
    info->huge_page_size = (integer)(info->huge_page_ratio * db_size);
    info->small_page_size = db_size - info->huge_page_size;
    if(info->small_page_size > 0){
        info->db_small_page = (char*)malloc(info->small_page_size * DBServer.rowSize);
        if(info->db_small_page == NULL){
            perror("malloc error");
            return -1;
        }
    }else{
        info->db_small_page = NULL;
    }
    void* ptr;
    if(info->huge_page_size > 0){
        ptr = mmap(NULL, info->huge_page_size * DBServer.rowSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
        if(ptr == MAP_FAILED){
            perror("mmap error");
            return -1;
        }
    }else{
        ptr = NULL;
    }
    info->db_huge_page = (char*)ptr;
    printf("small page size: %lld, huge page size: %lld \n", info->small_page_size, info->huge_page_size);
    memset(info->db_small_page, 'S', DBServer.rowSize * info->small_page_size);
    memset(info->db_huge_page, 'S', DBServer.rowSize * info->huge_page_size);

    return 0;
}

void db_myfork_hotcold_destroy(void *myfork_hotcold_info) {
    db_myfork_hotcold_infomation *info;
    info = myfork_hotcold_info;

    free(info->db_small_page);
    munmap(info->db_huge_page, info->huge_page_size * DBServer.rowSize);
}

void *myfork_hotcold_read(size_t index) {
    return NULL;
}

int myfork_hotcold_write(size_t page_index, void *value) {
    db_myfork_hotcold_infomation* info = &DBServer.myfork_hotcoldInfo;
    int rowSize = DBServer.rowSize;

    if(page_index < info->small_page_size){
        memcpy(info->db_small_page + page_index * rowSize, value, FILED_SIZE);
    }else{
        memcpy(info->db_huge_page + (page_index - info->small_page_size) * rowSize, value, FILED_SIZE);
    }

    return 0;
}

void ckp_myfork_hotcold(int ckp_order, void *myfork_hotcold_info) {
    FILE* ckp_fd;
    char ckp_name[32];
    db_myfork_hotcold_infomation *info;
    integer timeStart, timeEnd;
    info = myfork_hotcold_info;

    sprintf(ckp_name, "./ckp_backup/dump_%d", ckp_order);

    db_lock(&(DBServer.pre_lock));
    timeStart = get_ntime();
    pid_t pid = fork();
    if (0 != pid) {
        timeEnd = get_ntime();
        db_unlock(&(DBServer.pre_lock));
        add_prepare_log(&DBServer, timeEnd - timeStart);
        integer time1 = get_mtime();
        wait(NULL); // waiting for child process exit
        timeEnd = get_mtime();
        add_overhead_log(&DBServer, timeEnd - time1);

    } else {  // a child checkpoint process
        if (NULL == (ckp_fd = fopen(ckp_name, "w+b"))) {
            perror("checkpoint file open error,checkout if the ckp_backup "
                   "directory is exist");
            return;
        }
        // char* buf = (char*)malloc(1024L*1024*1024);
        // setvbuf(ckp_fd,buf,_IOFBF,1024L*1024*1024);
        setbuf(ckp_fd, NULL);

        if(info->small_page_size > fwrite(info->db_small_page, DBServer.rowSize, info->small_page_size, ckp_fd)){
            perror("write small page error");
            _exit(0);
        }
        if(info->huge_page_size > fwrite(info->db_huge_page, DBServer.rowSize, info->huge_page_size, ckp_fd)){
            perror("write huge page error");
            _exit(0);
        }

        fflush(ckp_fd);
        fclose(ckp_fd);
        _exit(0);
    }
}
