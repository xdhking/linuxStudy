#include "network.hpp"
#include<pthread.h>
#include<cstring>
#include<string>
using namespace std;
using namespace net;

void *send_routine(void* args){
    tcpClient *tc=static_cast<tcpClient*>(args);
    while(true){
        char buf[1024]={0};
        fgets(buf,sizeof(buf),stdin);
        tc->send(buf,strlen(buf));
    }
}

void *recv_routine(void* args){
    tcpClient *tc=static_cast<tcpClient*>(args);
    while(true){
        char buf[1024]={0};
        tc->recv(buf,sizeof(buf));
        printf("%s",buf);
    }
}

int main(){
    tcpClient *tc=new tcpClient();
    tc->initClient();
    pthread_t s,r;
    pthread_create(&s,nullptr,send_routine,tc);
    pthread_create(&r,nullptr,recv_routine,tc);

    pthread_join(s,nullptr);
    pthread_join(r,nullptr);
    delete tc;
}