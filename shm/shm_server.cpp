#include "comm.hpp"
int main(){
    key_t kid=getKey();
    std::cout<<"server key: "<<kid<<std::endl;
    int shmid=createShm(kid);
    std::cout<<"server shmid: "<<shmid<<std::endl;
    char *shm=(char*)attachShm(shmid);
    while(true){
        std::cout<<"client->server: "<<shm<<std::endl;
        sleep(1);
    }
    detachShm(shm);
    deleteShm(shmid);
    return 0;
}