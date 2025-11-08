#include<iostream>
#include<cstdio>
#include<cstring>
#include<unistd.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<cstdlib>
using namespace std;

void routine(int fd,struct sockaddr_in addr){
    char ip[64];
    char buf[64];
    inet_ntop(AF_INET,&addr.sin_addr,ip,sizeof(ip));
    snprintf(buf,sizeof(buf)-1,"[%s][%d]",ip,ntohs(addr.sin_port));
    string name=buf;    
    while(true){
        char buf[1024];
        ssize_t n=recv(fd,buf,sizeof(buf),0);
        if(n==0){
            close(fd);
            exit(0);
        }
        buf[n]='\0';
        cout<<name<<"# "<<buf<<endl;
        string s="收到："+(string)buf;
        send(fd,s.data(),s.size(),0);
    }
}

int main(){
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in local;
    memset(&local,0,sizeof(local));
    local.sin_family=AF_INET;
    local.sin_port=htons(8080);
    local.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(listenfd,(struct sockaddr*)&local,sizeof(local));
    listen(listenfd,5);
    while(true){
        struct sockaddr_in src;
        socklen_t len=sizeof(src);
        int fd=accept(listenfd,(struct sockaddr*)(&src),&len);
        pid_t pid=fork();
        if(pid==0){
            close(listenfd);
            if(fork()>0) exit(0);
            routine(fd,src);
            exit(0);
        }
        waitpid(pid,nullptr,0);
        close(fd);
        cout<<"wait success"<<endl;
    }
}