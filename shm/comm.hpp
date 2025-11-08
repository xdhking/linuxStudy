#pragma once
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<cerrno>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define SHM_PATH "."
#define PROJ_ID 0x66
#define MAX_SIZE 4096
int getKey(){
    key_t id=ftok(SHM_PATH,PROJ_ID);
    if(id==-1){
        std::cerr<<errno<<" : "<<strerror(errno)<<std::endl;
        exit(1);
    }
    return id;
}
int createShm(key_t id){
    int shmid=shmget(id,MAX_SIZE,IPC_CREAT|IPC_EXCL|0600);
    if(shmid<0){
        std::cerr<<errno<<" : "<<strerror(errno)<<std::endl;
        exit(2);
    }
    return shmid;
}
int getShmid(key_t id){
    int shmid=shmget(id,MAX_SIZE,IPC_CREAT);
    if(shmid<0){
        std::cerr<<errno<<" : "<<strerror(errno)<<std::endl;
        exit(3);
    }
    return shmid;
}

void deleteShm(int shmid){
    int ret=shmctl(shmid,IPC_RMID,nullptr);
    if(ret<0){
        std::cerr<<errno<<" : "<<strerror(errno)<<std::endl;
        exit(4);
    }
}

void *attachShm(int shmid){
    // 挂载共享内存（内核自动选择地址，读写权限）
    void* shm_p=shmat(shmid,NULL,0);
    if((long long)shm_p==-1L){
        std::cerr<<errno<<" : "<<strerror(errno)<<std::endl;
        exit(5);
    }
    return shm_p;
}

void detachShm(const void* shmaddr){
    int ret=shmdt(shmaddr);
    if(ret<0){
        std::cerr<<errno<<" : "<<strerror(errno)<<std::endl;
        exit(6);
    }
}