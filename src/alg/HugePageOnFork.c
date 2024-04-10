#include "src/include/util.h"


void ForkAndRecord(char* addr,int uf,int size) {
    integer timeStart, timeEnd;

    timeStart = get_ntime();
    pid_t pid = fork();
    if (0 != pid) {
        timeEnd = get_ntime();
        printf("%lld\n",timeEnd - timeStart);

        timeStart = get_ntime();
        for(int i = 0; i < uf; i++){
            addr[i*size] = ',';
        }
        timeEnd = get_ntime();
        printf("%lld\n",timeEnd - timeStart);
        wait(NULL); // waiting for child process exit
    } else {  // a child checkpoint process
        sleep(1);
        _exit(0);
    }
}

void generateRandomString(char *str, int length) {
    // 定义字符集
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charsetSize = sizeof(charset) - 1;

    // 为了每次运行程序时都能生成不同的随机字符串，需要设置随机种子
    srand(time(NULL));

    // 生成随机字符串
    for (int i = 0; i < length; ++i) {
        int key = rand() % charsetSize;
        str[i] = charset[key];
    }
}

int main(int argc, char *argv[]){
    // char *addr = (char*)malloc(1024*1024*512);
    // generateRandomString(addr, 1024*1024*512);
    // ForkAndRecord(addr, atoi(argv[1]),1024*4);
    // free(addr);
    
    char *addr = (char*)mmap(NULL, 1024*1024*512, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    generateRandomString(addr, 1024*1024*512);
    ForkAndRecord(addr,atoi(argv[1]),1024*1024*2);
    munmap(addr, 1024*1024*512);
    
    return 0;
}