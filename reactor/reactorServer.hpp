#include "log.hpp"
#include<iostream>
#include<string>
#include<functional>
#include<cstring>
#include<unistd.h>
#include<cerrno>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<unordered_map>

#define CH_NONBLOCK(fd) do{\
    int fl = fcntl(fd, F_GETFL);\
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);\
}while(0);


class connection{
    using func_t =std::function<void(connection*)>;
public:
    connection(const int &FD,func_t r=nullptr,func_t s=nullptr,func_t e=nullptr):\
    fd(FD),recver(r),sender(s),excepter(e){}
    void Close(){
        close(fd);
    }
    ~connection(){
        Close();
    }
public:
    std::string inbuffer;
    std::string outbuffer;
    int fd;
    func_t recver;
    func_t sender;
    func_t excepter;
};

const int defaultval=-1;
const uint16_t defaultport=8080;
const int defaultnum=1024;
class Server{
    using func_t =std::function<void(connection*)>;
public:
    Server(const uint16_t &port=defaultport,const int &num=defaultnum):\
    _port(port),_num(num),_listenfd(defaultval),_epfd(defaultval),_events(nullptr){}
    void initServer(){
        _listenfd=socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_addr.s_addr=INADDR_ANY;
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        bind(_listenfd,(struct sockaddr*)&local,sizeof(local));
        listen(_listenfd,32);
        _epfd=epoll_create(128);
        _events=new struct epoll_event[_num];
        addConnection(_listenfd,std::bind(&Server::Accepter,this,std::placeholders::_1),nullptr,nullptr);
    }
    void Accepter(connection* conn){
        while(true){
            struct sockaddr in;
            socklen_t len=sizeof(in);
            int fd=accept(conn->fd,&in,&len);
            if(fd<0){
                // 非阻塞模式下无连接：正常退出循环
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                // 被信号中断：重试
                else if (errno == EINTR)
                    continue;
                // 其他错误：日志+退出
                else
                {
                    logMessage(ERROR,"accept error");
                    break;
                }
            }
            addConnection(fd,std::bind(&Server::Recver,this,std::placeholders::_1),\
            std::bind(&Server::Sender,this,std::placeholders::_1),\
            std::bind(&Server::Excepter,this,std::placeholders::_1));
        }
    }
    void Recver(connection *conn){
        while(true){
            char buffer[1024];
            ssize_t n=recv(conn->fd,buffer,sizeof(buffer)-1,0);
            if(n==0){
                if(conn->excepter)
                    conn->excepter(conn);
                return;
            }
            else if(n<0){
                if(errno==EAGAIN||EWOULDBLOCK)
                    break;
                else if(errno==EINTR)
                    continue;
                else{
                    if(conn->excepter)
                        conn->excepter(conn);
                    return;
                }
            }
            buffer[n]='\0';
            // conn->inbuffer+=buffer;
            conn->outbuffer+=buffer;
        }
        std::cout<<"收到："<<conn->outbuffer<<std::endl;
        conn->sender(conn);
    }
    void Sender(connection *conn){
        while(true){
            ssize_t n=send(conn->fd,conn->outbuffer.c_str(),conn->outbuffer.size(),0);
            if(n==0){
                // if(conn->excepter)
                //     conn->excepter(conn);
                break;
            }
            else if(n<0){
                if(errno==EAGAIN||EWOULDBLOCK)
                    break;
                else if(errno==EINTR)
                    continue;
                else{
                    if(conn->excepter)
                        conn->excepter(conn);
                    return;
                }
            }
            conn->outbuffer.erase(0,n);
            if(conn->outbuffer.size()==0)
                break;
        }
        if(conn->outbuffer.size()==0){
            setRdWr(conn->fd,1,0);
        }
        else{
            setRdWr(conn->fd,1,1);
        }
    }
    void Excepter(connection *conn){
        epoll_ctl(_epfd,EPOLL_CTL_DEL,conn->fd,nullptr);
        conn->Close();
        _conns.erase(conn->fd);
        delete conn;
    }

    void setRdWr(int fd,bool r,bool w){
        struct epoll_event event={0};
        event.data.fd=fd;
        event.events=(r?EPOLLIN:0)|(w?EPOLLOUT:0)|EPOLLET;
        epoll_ctl(_epfd,EPOLL_CTL_MOD,fd,&event);
    }
    void addConnection(const int &fd,func_t r,func_t s,func_t e){
        connection *conn=new connection(fd,r,s,e);
        _conns[fd]=conn;
        CH_NONBLOCK(fd);
        struct epoll_event ev;
        memset(&ev,0,sizeof(ev));
        ev.data.fd=fd;
        ev.events=EPOLLIN|EPOLLET;
        epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&ev);
    }
    bool isExist(int fd){
        return _conns.find(fd)!=_conns.end();
    }
    void start(){
        while(true){
            epoll_wait(_epfd,_events,_num,-1);
            for(int i=0;i<_num;i++){
                int sock=_events[i].data.fd;
                uint32_t event=_events[i].events;
                if((event&EPOLLHUP)||(event&EPOLLERR)){
                    event=EPOLLIN|EPOLLOUT;
                }
                if((event&EPOLLIN)&&isExist(sock)&&_conns[sock]->recver)
                    _conns[sock]->recver(_conns[sock]);
                if((event&EPOLLIN)&&isExist(sock)&&_conns[sock]->sender)
                    _conns[sock]->sender(_conns[sock]);
            }
        }
    }
    ~Server(){
        if(_listenfd>=0){
            close(_listenfd);
        }
        if(_epfd>=0){
            close(_epfd);
        }
        if(_events!=nullptr){
            delete []_events;
        }
    }
private:
    std::unordered_map<int,connection*> _conns;
    int _epfd;
    struct epoll_event *_events;
    int _num;
    int _listenfd;
    uint16_t _port;
};