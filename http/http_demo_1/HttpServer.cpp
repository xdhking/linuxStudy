#include<iostream>
#include<cstring>
#include<unistd.h>
#include<pthread.h>
#include<cassert>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>
using namespace std;
class ThreadData{
    ;
};
void *routine (void *args){
    ThreadData *td=static_cast<ThreadData*>(args);
    while(true){
        ;
    }
    return nullptr;
}
int main (){
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in local;
    memset(&local,0,sizeof(local));
    local.sin_family=AF_INET;
    local.sin_addr.s_addr=htonl(INADDR_ANY);
    local.sin_port=htons(8080);
    bind(listenfd,(struct sockaddr*)(&local),sizeof(local));
    int n=listen(listenfd,5);
    assert(n==0);
    while(true){
        struct sockaddr_in addr;
        socklen_t len;
        int fd=accept(listenfd,(struct sockaddr*)(&addr),&len);
        pthread_t pid;
        ThreadData *td=new ThreadData;
        pthread_create(&pid,nullptr,routine,td);
    }
    return 0;
}