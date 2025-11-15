#pragma once
#include "log.hpp"
#include<cstring>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<netinet/ip.h>
#include<netinet/in.h>

#define defaultnum 1024
#define defaultval -1
#define defaultport 8080

class epollServer{
public:
    epollServer(const uint16_t &port=defaultport,const int &num=defaultnum):\
    _port(port),_num(num),_listenfd(-1),_events(nullptr){}
    void initServer(){
        _events=new epoll_event[_num];
        _listenfd=socket(AF_INET,SOCK_STREAM,0);
        if(_listenfd<0){
            logMessage(FATAL,"failed to create socket [errno:%s]",strerror(errno));
            exit(0);
        }
        logMessage(NORMAL,"socket create successfully [_listenfd:%d]",_listenfd);
        
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        local.sin_addr.s_addr=htonl(INADDR_ANY);
        bind(_listenfd,(struct sockaddr *)&local,sizeof(local));
        listen(_listenfd,32);
        _epfd=epoll_create(1);
        if(_epfd<0){
            logMessage(FATAL,"failed to create epfd [errno:%s]",strerror(errno));
        }
        logMessage(NORMAL,"epfd create successfully [epfd:%d]",_epfd);
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.data.fd=_listenfd;
        ev.events=EPOLLIN;
        epoll_ctl(_epfd,EPOLL_CTL_ADD,_listenfd,&ev);
    }

    void HandlerEvent(int event_num){
        for(int i=0;i<event_num;i++){
            int sock=_events[i].data.fd;
            uint32_t event=_events[i].events;
            if(sock==_listenfd&&(event&EPOLLIN)){
                struct sockaddr in;
                socklen_t len=sizeof(in);
                int fd=accept(_listenfd,&in,&len);
                if(fd!=-1) logMessage(NORMAL,"get a new fd");
                struct epoll_event ev;
                memset(&ev,0,sizeof(ev));
                ev.data.fd=fd;
                ev.events=EPOLLIN;
                epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&ev);
            }
            else if(event&EPOLLIN){
                char buffer[1024];
                int n=recv(sock,buffer,sizeof(buffer),0);
                if(n==0){
                    epoll_ctl(_epfd,EPOLL_CTL_DEL,sock,nullptr);
                    close(sock);
                }
                buffer[n-1]=0;
                logMessage(NORMAL,"receive new message: %s",buffer);
            }
            else{}
        }
    }

    void start(){
        for(;;){
            int n=epoll_wait(_epfd,_events,_num,-1);
            logMessage(DEBUG,"events is ready");
            switch(n){
            case 0:
            {
                logMessage(NORMAL,"timeout...");
            }
                break;
            case -1:
            {
                logMessage(FATAL,"wait error");
            }
                break;
            default:{
                HandlerEvent(n);
            }
            }
        }
    }
    ~epollServer(){
        if(_listenfd>=0) close(_listenfd);
        if(_events!=nullptr) delete []_events;
    }

private:
    int _epfd;
    int _listenfd;
    uint16_t _port;
    epoll_event *_events;
    int _num;
};