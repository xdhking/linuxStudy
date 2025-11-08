#include<iostream>
#include<cstdio>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
using namespace std;
int main(){
    int listen_sock=socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock<0){
        perror("listen_sock");
        return 0;
    }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8080);
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    int ret=bind(listen_sock,(struct sockaddr*)&addr,sizeof(addr));
    if(ret<0){
        perror("bind error");
        return 0;
    }
    ret=listen(listen_sock,1);
    if(ret<0){
        perror("listen");
        return 0;
    }
    while(true){
        struct sockaddr_in peer_addr;
        socklen_t peer_addrlen=sizeof(peer_addr);
        int new_sock=accept(listen_sock,(struct sockaddr*)&peer_addr,&peer_addrlen);
        if(new_sock<0){
            perror("accept");
            return 0;
        }
        printf("accept %s:%d, create new_sock %d\n",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port),new_sock);
        char buf[1024]={0};
        ssize_t recv_size=recv(new_sock,buf,sizeof(buf)-1,0);
        if(recv_size<0){
            perror("recv");
            return 0;
        }
    }
    return 0;
}