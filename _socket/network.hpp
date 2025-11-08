#pragma once
#include<iostream>
#include<cstdlib>
#include<string>
#include<functional>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include <strings.h>
#include<arpa/inet.h>
#define defaultPort 8080
#define defaultIp "123.56.19.236"
#define DFIP defaultIp
#define DFPT defaultPort

namespace net
{
    enum {SOCKET_ERR=1,BIND_ERR,RECV_ERR,SEND_ERR,LISTEN_ERR,ACCEPT_ERR,CONNECT_ERR};
    class udpServer{
    public:
        udpServer(const uint16_t &port=DFPT):_port(port){}
        void initServer(){
            _fd=socket(AF_INET,SOCK_DGRAM,0);
            if(_fd<0){
                perror("socket");
                exit(SOCKET_ERR);
            }
            struct sockaddr_in local;
            bzero(&local,sizeof(local));
            local.sin_family=AF_INET;
            local.sin_port=htons(_port);
            local.sin_addr.s_addr=htonl(INADDR_ANY);
            int n=bind(_fd,(struct sockaddr*)&local,sizeof(local));
            if(n<0){
                perror("bind");
                exit(BIND_ERR);
            }
        }
        void close(){
            ::close(_fd);
        }
        ~udpServer(){
            close();
        }
        ssize_t recv(void *buf,struct sockaddr_in &src_addr,const size_t &maxlen){
            socklen_t len=sizeof(src_addr);
            int n=recvfrom(_fd,buf,maxlen,0,(struct sockaddr*)&src_addr,&len);
            if(n<0){
                perror("recv");
                exit(RECV_ERR);
            }
            return n;
        }
        ssize_t send(const void *buf,const struct sockaddr_in &des_addr,const size_t &dataLen){
            socklen_t len=sizeof(des_addr);
            int n=sendto(_fd,buf,dataLen,0,(struct sockaddr*)&des_addr,len);
            if(n<0){
                perror("send");
                exit(SEND_ERR);
            }
            return n;
        }
    private:
        int _fd;
        uint16_t _port;
    };



    class udpClient{
    public:
        udpClient(const char* server_ip=DFIP,const uint16_t server_port=DFPT):_server_ip(server_ip),_server_port(server_port){
            bzero(&_addr,sizeof(_addr));
            _addr.sin_family=AF_INET;
            _addr.sin_port=htons(_server_port);
            _addr.sin_addr.s_addr=inet_addr(_server_ip.c_str());
        }
        void close(){
            ::close(_fd);
        }
        ~udpClient(){
            close();
        }
        void initClient(){
            _fd=socket(AF_INET,SOCK_DGRAM,0);
            if(_fd<0){
                perror("socket");
                exit(SOCKET_ERR);
            }
        }
        ssize_t recv(void *buf,const size_t &maxlen){
            struct sockaddr_in src_addr;
            socklen_t len=sizeof(src_addr);
            int n=recvfrom(_fd,buf,maxlen,0,(struct sockaddr*)&src_addr,&len);
            if(n<0){
                perror("recv");
                exit(RECV_ERR);
            }
            return n;
        }
        ssize_t send(const void *buf,const size_t &dataLen){
            socklen_t len=sizeof(_addr);
            int n=sendto(_fd,buf,dataLen,0,(struct sockaddr*)&_addr,len);
            if(n<0){
                perror("send");
                exit(SEND_ERR);
            }
            return n;
        }
    private:
        int _fd;
        uint16_t _server_port;
        std::string _server_ip;
        struct sockaddr_in _addr;
    };

    class tcpChild{
    public:
        tcpChild(const int &fd,const struct sockaddr_in &addr):_fd(fd),_addr(addr){}
        void close(){
            ::close(_fd);
        }
        ~tcpChild(){
            close();
        }
        struct sockaddr_in get_addr(){
            return _addr;
        }
        uint16_t get_port(){
            uint16_t ret=ntohs(_addr.sin_port);
            return ret;
        }
        std::string get_ip(){
            char buf[64];
            std::string ret;
            const char *p=inet_ntop(AF_INET,&_addr.sin_addr,buf,sizeof(buf));
            ret=buf;
            return ret;
        }
        ssize_t recv(void *buf,const size_t &maxlen){
            ssize_t ret=::recv(_fd,buf,maxlen,0);
            if(ret==0){
                close();
            }
            if(ret<0){
                perror("recv");
                exit(RECV_ERR);
            }
            return ret;
        }

        ssize_t send(const void *buf,const size_t len){
            ssize_t ret=::send(_fd,buf,len,0);
            if(ret<0){
                std::cerr<<"tcp send error"<<std::endl;
                close();
            }
            return ret;
        }
    private:
        int _fd;
        struct sockaddr_in _addr;
    };

    class tcpServer{
    public:
        tcpServer(const uint16_t &port=DFPT,const int &backlog=5):_listen_port(port),_backlog(backlog){}
        void close(){
            ::close(_listen_fd);
        }
        ~tcpServer(){
            close();
        }
        void initServer(){
            _listen_fd=socket(AF_INET,SOCK_STREAM,0);
            if(_listen_fd<0){
                perror("socket");
                exit(SOCKET_ERR);
            }
            struct sockaddr_in local;
            bzero(&local,sizeof(local));
            local.sin_family=AF_INET;
            local.sin_port=htons(_listen_port);
            local.sin_addr.s_addr=htonl(INADDR_ANY);
            int n=bind(_listen_fd,(struct sockaddr*)&local,sizeof(local));
            if(n<0){
                perror("bind");
                exit(BIND_ERR);
            }
            n=listen(_listen_fd,_backlog);
            if(n<0){
                perror("listen");
                exit(LISTEN_ERR);
            }
        }

        tcpChild start_accept(){
            struct sockaddr_in addr;
            socklen_t len;
            int fd=accept(_listen_fd,(struct sockaddr*)&addr,&len);
            if(fd<0){
                perror("accept");
                exit(ACCEPT_ERR);
            }
            return tcpChild(fd,addr);
        }

    private:
        int _listen_fd;
        int _backlog;
        uint16_t _listen_port;
    };

    class tcpClient{
    public:
        tcpClient(const char* ip=DFIP,const uint16_t &port=DFPT):_ip(ip),_port(port){}
        void close(){
            ::close(_fd);
        }
        ~tcpClient(){
            close();
        }
        void initClient(){
            _fd=socket(AF_INET,SOCK_STREAM,0);
            if(_fd<0){
                perror("socket");
                exit(SOCKET_ERR);
            }
            struct sockaddr_in addr;
            bzero(&addr,sizeof(addr));
            addr.sin_addr.s_addr=inet_addr(_ip.c_str());
            addr.sin_family=AF_INET;
            addr.sin_port=htons(_port);
            int n=connect(_fd,(struct sockaddr*)&addr,sizeof(addr));
            if(n<0){
                perror("connect");
                exit(CONNECT_ERR);
            }
        }

        ssize_t recv(void *buf,const size_t &maxlen){
            ssize_t ret=::recv(_fd,buf,maxlen,0);
            if(ret==0){
                close();
            }
            if(ret<0){
                perror("recv");
                exit(RECV_ERR);
            }
            return ret;
        }

        ssize_t send(const void *buf,const size_t len){
            ssize_t ret=::send(_fd,buf,len,0);
            if(ret<0){
                std::cerr<<"tcp send error"<<std::endl;
                close();
            }
            return ret;
        }
    private:
        int _fd;
        std::string _ip;
        uint16_t _port;
    };
}