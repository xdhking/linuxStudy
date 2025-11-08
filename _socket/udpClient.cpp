#include "network.hpp"
#include<memory>
#include<pthread.h>
#include<vector>
#include<unordered_map>
#include<signal.h>
#include<cstring>
void *recv_runtine(void *args){
    net::udpClient *client=static_cast<net::udpClient*>(args);
    while(true){
        char buffer[1024]={0};
        client->recv(buffer,sizeof(buffer)-1);
        char ip[64]={0};
        printf("[server]# %s\n",buffer);
    }
}
void *send_runtine(void *args){
    net::udpClient *client=static_cast<net::udpClient*>(args);
    char buffer[1024]={0};
    while(true){
        std::cout<<"输入对话# ";
        std::cin>>buffer;
        client->send(buffer,strlen(buffer));
    }
}
int main(){
    net::udpClient *client=new net::udpClient();
    // std::unique_ptr<net::udpServer> server(new net::udpServer());
    client->initClient();
    pthread_t r,s;
    pthread_create(&r,nullptr,recv_runtine,(void*)client);
    pthread_create(&s,nullptr,send_runtine,(void*)client);

    pthread_join(r,nullptr);
    pthread_join(s,nullptr);
    delete client;
}