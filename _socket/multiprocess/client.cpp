#include<iostream>
#include<cstring>
#include<pthread.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
using namespace std;
int main(){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8080);
    addr.sin_addr.s_addr=inet_addr("123.56.19.236");
    connect(fd,(struct sockaddr*)&addr,sizeof(addr));
    while(true){
        char buf[1024];
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1]='\0';
        int n=send(fd,buf,strlen(buf),0);
        if(n<0){
            exit(0);
        }
        n=recv(fd,buf,sizeof(buf)-1,0);
        buf[n]='\0';
        printf("%s\n",buf);
    }
    return 0;
}