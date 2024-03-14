#include "src/include/system.h"
#include "src/include/myfork_lru.h"

extern db_server DBServer;

// DoubleList* createDoubleList(){
//     DoubleList* list = (DoubleList*)malloc(sizeof(DoubleList));
//     list->head = (node*)malloc(sizeof(node));
//     list->tail = (node*)malloc(sizeof(node));
//     list->head->prev = NULL;
//     list->head->next = list->tail;
//     list->tail->prev = list->head;
//     list->tail->next = NULL;
//     list->size = 0;
//     return list;
// }

// void addLast(DoubleList* list, node* x){//将节点x插入到双链表的尾部
//     x->next = list->tail;
//     x->prev = list->tail->prev;
//     x->prev->next = x;
//     list->tail->prev = x;
//     list->size++;
// }

// void removeX(DoubleList* list, node* x){//将节点x从双链表中删除，但是不释放节点x的内存
//     x->next->prev = x->prev;
//     x->prev->next = x->next;
//     list->size--;
// }

// node* removeFirst(DoubleList* list){//删除链表中第一个节点，并返回该节点
//     if(list->head->next == list->tail){
//         return NULL;
//     }
//     node* first = list->head->next;
//     removeX(list, first);
//     return first;
// }

// int getSize(DoubleList* list){
//     return list->size;
// }

// LRUCache* createLRUCache(int capacity){
//     LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
//     cache->capacity = capacity;
//     cache->map = (node**)malloc(sizeof(node*) * DBServer.dbSize);
//     cache->list = createDoubleList();
//     return cache;
// }

// void makeRecently(LRUCache* cache, integer index){//将缓存中的某个key提升为最近使用的
//     node* x = cache->map[index];
//     removeX(cache->list, x);
//     addLast(cache->list, x);
// }

// void addRecently(LRUCache* cache, integer index, char* buf){//将某个key加入缓存
//     node* x = (node*)malloc(sizeof(node));
//     x->index = index;
//     x->buf = buf;
//     cache->map[index] = x;
//     addLast(cache->list, x);
// }

// void deleteKey(LRUCache* cache, integer index){//将某个key从缓存中删除，并释放内存
//     node* x = cache->map[index];
//     if(x == NULL){
//         return;
//     }
//     removeX(cache->list, x);
//     cache->map[index] = NULL;
//     free(x->buf);
//     free(x);
// }

// node* removeLeastRecently(LRUCache* cache){//删除最久未使用的元素，并返回该元素
//     node* x = removeFirst(cache->list);
//     if(x != NULL){
//         cache->map[x->index] = NULL;
//         return x;
//     }
//     return NULL;
// }

// char* get(LRUCache* cache, integer index){//读
//     node* x = cache->map[index];
//     if(x == NULL){
//         return NULL;
//     }
//     makeRecently(cache, index);
//     return x->buf;
// }

// node* put(LRUCache* cache, integer index, char* buf){//写
//     node* x = cache->map[index];
//     if(x != NULL){
//         free(x->buf);
//         x->buf = buf;
//         makeRecently(cache, index);
//         return NULL;
//     }
//     if(cache->capacity == getSize(cache->list)){
//         return removeLeastRecently(cache);
//     }
//     addRecently(cache, index, buf);
//     return NULL;
// }

int db_myfork_lru_init(void *myfork_lru_info, size_t db_size) {
    db_myfork_lru_infomation *info;
    info = myfork_lru_info;

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

    memset(info->db_small_page, 'S', DBServer.rowSize * info->small_page_size);
    memset(info->db_huge_page, 'S', DBServer.rowSize * info->huge_page_size);

    // int capacity = db_size - info->huge_page_size;
    // info->db_small_page = createLRUCache(capacity);
    // info->addr_map = (integer *) malloc(db_size * sizeof(integer));
    // info->vis_count = (int*)malloc(db_size * sizeof(int));

    // memset(info->db_huge_page, 'S', info->huge_page_size * DBServer.rowSize);
    // int i;
    // for(i = 0; i < info->db_small_page->capacity; i++){
    //     char* buf = (char*)malloc(DBServer.rowSize);
    //     memset(buf, 'S', DBServer.rowSize);
    //     put(info->db_small_page, i, buf);
    //     info->addr_map[i] = -1;//-1表示在小页面中
    // }
    // for(i = capacity; i < db_size; i++){
    //     info->addr_map[i] = i - capacity;//表示在大页面中
    // }

    return 0;
}

void db_myfork_lru_destroy(void *myfork_lru_info) {
    db_myfork_lru_infomation *info;
    info = myfork_lru_info;

    free(info->db_small_page);
    munmap(info->db_huge_page, info->huge_page_size * DBServer.rowSize);
    // int i;
    // for(i = 0; i < info->db_size; i++){
    //     if(info->addr_map[i] == -1){
    //         deleteKey(info->db_small_page, i);
    //     }
    // }
    // free(info->db_small_page->list->head);
    // free(info->db_small_page->list->tail);
    // free(info->db_small_page->list);
    // free(info->db_small_page->map);
    // free(info->db_small_page);
    // free(info->addr_map);
    // free(info->vis_count);
}

void *myfork_lru_read(size_t index) {
    // void *result;
    // integer real_index = DBServer.myfork_lruInfo.addr_map[index];
    // if(real_index == -1){
    //     result = get(DBServer.myfork_lruInfo.db_small_page, index);
    // }else{
    //     result = (void*)((DBServer.myfork_lruInfo).db_huge_page + real_index * DBServer.rowSize);
    // }

    // return result;
    return NULL;
}

int myfork_lru_write(size_t page_index, void *value) {
    db_myfork_lru_infomation* info = &DBServer.myfork_lruInfo;
    int rowSize = DBServer.rowSize;

    if(page_index < info->small_page_size){
        memcpy(info->db_small_page + page_index * rowSize, value, FILED_SIZE);
    }else{
        memcpy(info->db_huge_page + (page_index - info->small_page_size) * rowSize, value, FILED_SIZE);
    }

    // integer real_index = info->addr_map[page_index];
    // if(real_index == -1){
    //     char* buf = (char*)malloc(rowSize);
    //     memcpy(buf, value, FILED_SIZE);
    //     put(DBServer.myfork_lruInfo.db_small_page, page_index, buf);
    // }else{
    //     memcpy((DBServer.myfork_lruInfo).db_huge_page + real_index * rowSize, value, FILED_SIZE);
    // }

    // int cnt = ++(info->vis_count[page_index]);
    // if(real_index == -1){
    //     char* buf = (char*)malloc(rowSize);
    //     memcpy(buf, value, FILED_SIZE);
    //     put(DBServer.myfork_lruInfo.db_small_page, page_index, buf);
    // }else{
    //     if(cnt < 100){//大页面中的数据距离上次快照的访问次数小于100，判断为冷数据，直接更新
    //         memcpy((DBServer.myfork_lruInfo).db_huge_page + real_index * rowSize, value, FILED_SIZE);
    //     }else{//大页面中的数据距离上次快照的访问次数大于100，判断为热数据，将其放入小页面中
    //         char* buf = (char*)malloc(rowSize);
    //         memcpy(buf, value, FILED_SIZE);
    //         node* rem = put(DBServer.myfork_lruInfo.db_small_page, page_index, buf);
    //         DBServer.myfork_lruInfo.addr_map[page_index] = -1;
    //         if(rem != NULL){
    //             DBServer.myfork_lruInfo.addr_map[rem->index] = real_index;
    //             memcpy((DBServer.myfork_lruInfo).db_huge_page + real_index * rowSize, rem->buf, FILED_SIZE);
    //             free(rem->buf);
    //             free(rem);
    //         }
    //     }
    // }

    return 0;
}

void ckp_myfork_lru(int ckp_order, void *myfork_lru_info) {
    FILE* ckp_fd;
    char ckp_name[32];
    db_myfork_lru_infomation *info;
    integer timeStart, timeEnd;
    info = myfork_lru_info;

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

        fwrite(info->db_small_page, info->small_page_size * DBServer.rowSize, 1, ckp_fd);
        fwrite(info->db_huge_page, info->huge_page_size * DBServer.rowSize, 1, ckp_fd);

        fflush(ckp_fd);
        fclose(ckp_fd);
        _exit(0);
        // ckp_fd = open(ckp_name, O_CREAT | O_RDWR, 0600);
        // if(ckp_fd == -1){
        //     perror("checkpoint file open error,checkout if the ckp_backup directory "
        //         "is exist");
        //     return;
        // }
        // if (ftruncate(ckp_fd, DBServer.rowSize * db_size) == -1) {
        //     perror("ftruncate failed");
        //     return;
        // }
        // void* db_mm_addr = mmap(NULL, DBServer.rowSize * db_size, PROT_READ | PROT_WRITE, MAP_SHARED, ckp_fd, 0);
        // memcpy(db_mm_addr, info->db_small_page, DBServer.rowSize * info->small_page_size);
        // memcpy(db_mm_addr + info->small_page_size * DBServer.rowSize, info->db_huge_page, DBServer.rowSize * info->huge_page_size);
        // msync(db_mm_addr, DBServer.rowSize * db_size, MS_SYNC);
        // int i;
        // for(i = 0; i < db_size; i++){
        //     if(info->addr_map[i] == -1){
        //         memcpy(db_mm_addr + i * DBServer.rowSize, get(info->db_small_page, i), DBServer.rowSize);
        //     }else{
        //         memcpy(db_mm_addr + i * DBServer.rowSize, (info->db_huge_page) + info->addr_map[i] * DBServer.rowSize, DBServer.rowSize);
        //     }
        // }
        // munmap(db_mm_addr, DBServer.rowSize * db_size);
        // close(ckp_fd);
        // memset(info->vis_count, 0, db_size * sizeof(int));
        // _exit(0);
    }
}
