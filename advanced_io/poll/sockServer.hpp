#pragma once
#include<iostream>
#include<algorithm>
#include "log.hpp"
#include<functional>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
#include<poll.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/in.h>
class server{
#define FD_NUM (sizeof(fd_set)*8)
#define defaultval -1
    using func_t=std::function<int(int)>;
public:
    server(const uint16_t &port):_port(port){
        fds=new pollfd[FD_NUM];
        for(int i=0;i<FD_NUM;i++){
            fds[i].fd=defaultval;
            fds[i].events=0;
            fds[i].revents=0;
        }
        _listenfd=socket(AF_INET,SOCK_STREAM,0);
        fds[0].fd=_listenfd;
        fds[0].events=POLLIN;
    }
    void init(){
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_addr.s_addr=htonl(INADDR_ANY);
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        bind(_listenfd,(struct sockaddr*)&local,sizeof(local));
        listen(_listenfd,32);
    }
    void insert(int fd){
        for(int i=1;i<FD_NUM;i++){
            if(fds[i].fd!=defaultval) continue;
            fds[i].fd=fd;
            fds[i].events=POLLIN;
            return;
        }
        logMessage(WARNING,"fdarry is full");
        close(fd);
    }
    void start(func_t routine){
        while(true){
            int n=poll(fds,FD_NUM,-1);
            if(n==0){
                logMessage(NORMAL,"timeout...");
            }
            else if(n<0){
                logMessage(ERROR,"error");
            }
            else{
                if(fds[0].fd==_listenfd&&(fds[0].revents&POLLIN)){
                    struct sockaddr in;
                    socklen_t len=sizeof(in);
                    int fd=accept(_listenfd,&in,&len);
                    insert(fd);
                }
                for(int i=1;i<FD_NUM;i++){
                    if(fds[i].fd==defaultval) continue;
                    if(fds[i].revents&POLLIN){
                        int ret=routine(fds[i].fd);
                        if(ret==0){
                            close(fds[i].fd);
                            fds[i].fd=defaultval;
                            fds[i].events=0;
                            fds[i].revents=0;
                        }
                    }
                }
            }
        }
    }
    ~server(){
        close(_listenfd);
        delete []fds;
    }
private:
    int _listenfd;
    uint16_t _port;
    pollfd *fds;
    int maxfd;
};