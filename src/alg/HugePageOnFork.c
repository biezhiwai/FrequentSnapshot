#include "src/include/util.h"


void ForkAndRecord(char* addr) {
    integer timeStart, timeEnd;

    timeStart = get_ntime();
    pid_t pid = fork();
    if (0 != pid) {
        timeEnd = get_ntime();
        printf("%lld\n",timeEnd - timeStart);
        int cnt = 32;

        timeStart = get_ntime();
        for(int i = 0; i < cnt; i++){
            printf("%c\n",addr[i*1024*1024*2]);

        }
        timeEnd = get_ntime();
        printf("%lld\n",(timeEnd - timeStart)/cnt);
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
    char *addr = (char*)malloc(1024*1024*atoi(argv[1]));
    // char *addr = (char*)mmap(NULL, 1024*1024*atoi(argv[1]), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    generateRandomString(addr, 1024*1024*atoi(argv[1]));
    ForkAndRecord(addr);
    free(addr);
    // munmap(addr, 1024*1024*atoi(argv[1]));
    
    return 0;
}