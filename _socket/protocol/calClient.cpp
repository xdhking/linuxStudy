#include "protocol.hpp"
#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/ip.h>
#include<netinet/in.h>
using namespace std;
int main(){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8080);
    addr.sin_addr.s_addr=inet_addr("123.56.19.236");
    connect(fd,(struct sockaddr*)&addr,sizeof(addr));
    string buf;
    cout<<"输入格式：x op y"<<endl;
    while(true){
        int x,y;
        char op;
        cin>>x>>op>>y;
        request rq(x,y,op);
        //序列化
        string ret=rq.serialize();
        //打包
        package(ret,ret);
        for(int i=0;i<(int)ret.size();i+=SZ){
            string tmp;
            if(i+SZ-1<ret.size())
                tmp=ret.substr(i,SZ);
            else
                tmp=ret.substr(i);
            send(fd,tmp.c_str(),tmp.size(),0);
        }
        ret.clear();
        if(!getMessage(buf,fd,ret)){
            return 0;
        }

        //提取数据部分
        uncover(ret,ret);

        response rp;
        //反序列化
        rp.deserialize(ret);

        //结果
        if(rp._exitcode==0){
            cout<<rp._result<<endl;
        }
        else
        cout<<"error"<<endl;
    }
}