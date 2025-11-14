#pragma once
#include "log.hpp"
#include<functional>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/in.h>
class server{
#define FD_NUM (sizeof(fd_set)*8)
#define defaultval -1
    using func_t=std::function<int(int,int)>;
public:
    server(const uint16_t &port):_port(port){
        fdarry=new int[FD_NUM];
        for(int i=0;i<FD_NUM;i++){
            fdarry[i]=defaultval;
        }
    }
    void init(){
        _listenfd=socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_addr.s_addr=htonl(INADDR_ANY);
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        int ret;
        ret=bind(_listenfd,(struct sockaddr*)&local,sizeof(local));
    }
    void start(func_t routine){
        ;
    }
    ~server(){}
private:
    int _listenfd;
    uint16_t _port;
    int *fdarry;
};