#include "comm.hpp"
int main(){
    key_t kid=getKey();
    int shmid=getShmid(kid);
    char *shm=(char*)attachShm(shmid);
    int cnt;
    std::cin>>cnt;
    const char *text="cnt目前为：";
    while(cnt--){
        snprintf(shm,MAX_SIZE,"发送消息，%s[%d]",text,cnt);
        sleep(1);
    }
    detachShm(shm);
    return 0;
}