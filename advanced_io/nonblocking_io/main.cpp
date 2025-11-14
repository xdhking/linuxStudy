#include<iostream>
#include<cstring>
#include<unistd.h>
#include "log.hpp"
#include<fcntl.h>
// #include<pthread.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
using namespace std;

// void *routine(void *args){
//     int fd=(int)(args);
//     while(true){

//     }
// }

int main(int argc,char *argv[]){
    if(argc!=2){
        logMessage(ERROR,"number of argument is error");
        return 0;
    }
    uint16_t port=atoi(argv[1]);
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    //改非阻塞io
    int fl=fcntl(listenfd,F_GETFL);
    fcntl(listenfd,F_SETFL,fl|O_NONBLOCK);
    struct sockaddr_in local;
    memset(&local,0,sizeof(local));
    local.sin_family=AF_INET;
    local.sin_port=htons(port);
    local.sin_addr.s_addr=htonl(INADDR_ANY);
    listen(listenfd,32);
    while(true){
        struct sockaddr_in client;
        socklen_t len=sizeof(client);
        int fd=accept(listenfd,(struct sockaddr*)&client,&len);
        logMessage(NORMAL,"accept successfully, [fd:%d]",fd);
        // pthread_t pid;
        // pid=pthread_create(&pid,nullptr,routine,(void*)fd);
        close(fd);
    }
    return 0;
}