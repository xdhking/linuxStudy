#include "network.hpp"
#include<memory>
#include<pthread.h>
#include<vector>
#include<unordered_map>
#include<signal.h>
#include<cstring>
std::vector<sockaddr_in> addr;
std::unordered_map<uint32_t,bool> exist; 
void show_client(int num){
    for(int i=0;i<addr.size();i++){
        char ip[64]={0};
        inet_ntop(AF_INET,&addr[i].sin_addr,ip,sizeof(ip));
        uint16_t port=ntohs(addr[i].sin_port);
        printf("addr[%d]: [%s][%d] %s\n",i,ip,port);
    }
}
void *recv_runtine(void *args){
    net::udpServer *server=static_cast<net::udpServer*>(args);
    while(true){
        sockaddr_in src;
        char buffer[1024]={0};
        server->recv(buffer,src,sizeof(buffer)-1);
        char ip[64]={0};
        inet_ntop(AF_INET,&src.sin_addr,ip,sizeof(ip));
        uint16_t port=ntohs(src.sin_port);
        if(!exist[src.sin_addr.s_addr]){
            addr.push_back(src);
            exist[src.sin_addr.s_addr]=true;
        }
        printf("[%s][%d]# %s\n",ip,port,buffer);
    }
}
void *send_runtine(void *args){
    net::udpServer *server=static_cast<net::udpServer*>(args);
    signal(20,show_client);
    char buffer[1024]={0};
    while(true){
        int id;
        std::cout<<"输入目的主机编号# ";
        std::cin>>id;
        std::cout<<"输入对话# ";
        std::cin>>buffer;
        server->send(buffer,addr[id],strlen(buffer));
    }
}
int main(){
    net::udpServer *server=new net::udpServer();
    // std::unique_ptr<net::udpServer> server(new net::udpServer());
    server->initServer();
    sockaddr_in tmp;
    bzero(&tmp,sizeof(tmp));
    tmp.sin_family=AF_INET;
    tmp.sin_port=htons(8080);
    tmp.sin_addr.s_addr=inet_addr("127.0.0.1");
    addr.push_back(tmp);
    pthread_t r,s;
    pthread_create(&r,nullptr,recv_runtine,(void*)server);
    pthread_create(&s,nullptr,send_runtine,(void*)server);

    pthread_join(r,nullptr);
    pthread_join(s,nullptr);
    delete server;
}