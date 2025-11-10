#include "Task.hpp"
#include "ThreadPool.hpp"
#include "protocol.hpp"
#include<iostream>
#include<unistd.h>
#include<cassert>
#include<cstdio>
#include<cstring>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>
using namespace std;
enum {DIV_ZERO=1,MOD_ZERO,OP_ERROR};
response cal(request &req){
    response resp;
    resp._exitcode=0;
    resp._result=0;
    switch (req._op)
    {
    case '+':
        resp._result = req._x + req._y;
        break;
    case '-':
        resp._result = req._x - req._y;
        break;
    case '*':
        resp._result = req._x * req._y;
        break;
    case '/':
    {
        if (req._y == 0)
            resp._exitcode = DIV_ZERO;
        else
            resp._result = req._x / req._y;
    }
    break;
    case '%':
    {
        if (req._y == 0)
            resp._exitcode = MOD_ZERO;
        else
            resp._result = req._x % req._y;
    }
    break;
    default:
        resp._exitcode = OP_ERROR;
        break;
    }

    return resp;
}

void routine(int sock,struct sockaddr_in src){
    std::string buf;
    while(true){
        std::string ret;
        //提取完整报文
        if(!getMessage(buf,sock,ret)){
            close(sock);
            return;
        }
        cout<<"原始报文#"<<endl<<ret;

        //拆解报文首部，留取数据部分
        int n=uncover(ret,ret);
        cout<<"报文数据部分#"<<endl<<ret<<endl;

        //将报文反序列化
        request rq;
        rq.deserialize(ret);
        cout<<"反序列化的结果#"<<endl;
        cout<<rq._x<<endl;
        cout<<rq._op<<endl;
        cout<<rq._y<<endl;

        //计算结果
        response rp=cal(rq);
        cout<<rp._result<<endl;
        cout<<rp._exitcode<<endl;
        
        //将结果序列化并打包成报文
        ret=rp.serialize();
        package(ret,ret);
        for(int i=0;i<(int)ret.size();i+=SZ){
            string tmp;
            if(i+SZ-1<ret.size())
                tmp=ret.substr(i,SZ);
            else
                tmp=ret.substr(i);
            send(sock,tmp.c_str(),tmp.size(),0);
        }
    }
}
int main(){
    ThreadPool<Task> *tp=ThreadPool<Task>::getInstance();
    int fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in local;
    memset(&local,0,sizeof(local));
    local.sin_addr.s_addr=htonl(INADDR_ANY);
    local.sin_family=AF_INET;
    local.sin_port=htons(8080);
    int ret=bind(fd,(struct sockaddr*)&local,sizeof(local));
    assert(ret==0);
    listen(fd,5);
    while(true){
        struct sockaddr_in src;
        socklen_t len =sizeof(src);
        ret=accept(fd,(struct sockaddr*)&src,&len);
        if(ret<0){
            exit(0);
        }
        tp->Push(Task(routine,ret,src));
    }
    close(fd);
    delete tp;
}