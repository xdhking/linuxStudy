#include "network.hpp"
#include<pthread.h>
#include<cstring>
#include<string>
using namespace std;
using namespace net;
//发送程序
void *send_routine(void* args){
    tcpClient *tc=static_cast<tcpClient*>(args);
    while(true){
        char buf[1024]={0};
        printf(">>>");
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1]='\0';
        tc->send(buf,strlen(buf));
    }
}
//接收程序
void *recv_routine(void* args){
    tcpClient *tc=static_cast<tcpClient*>(args);
    while(true){
        char buf[1024]={0};
        int n=tc->recv(buf,sizeof(buf));
        buf[n]='\0';
        printf("%s\n",buf);
    }
}

int main(){
    tcpClient *tc=new tcpClient();
    tc->initClient();
    pthread_t s,r;
    //客户端没啥技术含量，连接上了就开两个线程，一个接受一个发送
    pthread_create(&s,nullptr,send_routine,tc);
    pthread_create(&r,nullptr,recv_routine,tc);

    pthread_join(s,nullptr);
    pthread_join(r,nullptr);
    delete tc;
}