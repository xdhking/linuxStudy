#include "log.hpp"
#include "sockServer.hpp"
#include<iostream>
#include<cstdlib>
#include<memory>
#include<sys/socket.h>
using namespace std;

int routine(int sock){
    char buffer[1024];
    int n=recv(sock,buffer,sizeof(buffer)-1,0);
    if(n==0){
        return 0;
    }
    buffer[n-1]='\0';
    cout<<"client# "<<buffer<<endl;
    logMessage(NORMAL,"receive is successful");
    send(sock,"已收到",strlen("已收到"),0);
    return 1;
}

int main(int argc,char *argv[]){
    if(argc!=2){
        logMessage(ERROR,"number of argument is error");
        return 0;
    }
    uint16_t port=atoi(argv[1]);
    unique_ptr<server> sv(new server(port));
    sv->init();
    sv->start(routine);
    return 0;
}