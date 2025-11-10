#pragma once
#include<functional>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
class Task{
    using func_t=std::function<void(int,struct sockaddr_in)>;
public:
    Task(){}
    Task(func_t f,const int &sk,struct sockaddr_in ar):fun(f),sock(sk),addr(ar){}
    void operator()(){
        fun(sock,addr);
    }
private:
    func_t fun;
    int sock;
    struct sockaddr_in addr;
};