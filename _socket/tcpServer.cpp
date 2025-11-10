#include "network.hpp"
#include<pthread.h>
#include<queue>
#include<string>
#include<unordered_map>
#include<cstring>
// #include "daemon.hpp"
using namespace std;
using namespace net;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
queue<pair<string,string>> msg;
unordered_map<string,tcpChild*> mp;
void *brodcast(void* args){
    while(true){
        pthread_mutex_lock(&mutex);
        while(msg.size()==0){
            //判断队列是否为空
            pthread_cond_wait(&cond,&mutex);
        }
        pair<string,string> tmp=msg.front();
        msg.pop();
        char buf[1024]={0};
        snprintf(buf,sizeof(buf)-1,"%s: %s",tmp.first.c_str(),tmp.second.c_str());
        //vector用于保存掉线的客户端地址
        vector<string> del;
        for(auto &client:mp){
            string key=client.first;
            tcpChild *tc=client.second;
            //如果key与发送该消息的客户端的地址相同则忽略
            if(key==tmp.first)
                continue;
            int n=tc->send(buf,strlen(buf));
            if(n<0){
                //如果发送失败，说明对方掉线，将改客户端地址存入del，然后delete掉tc
                del.push_back(key);
                delete tc;
            }
        }
        for(int i=0;i<del.size();i++){
            //将掉线的客户端从mp中移除
            mp.erase(del[i]);
        }
        pthread_mutex_unlock(&mutex);
    }
}

void *recv_routine(void *args){
    // daemonSelf();
    tcpChild *tc=static_cast<tcpChild*>(args);
    //格式[0.0.0.0][1111]
    string client_addr="["+tc->get_ip()+"]"+"["+to_string(tc->get_port())+"]";
    pthread_mutex_lock(&mutex);
    mp[client_addr]=tc;//将新的客户端塞进哈希表
    pthread_mutex_unlock(&mutex);
    char buf[1024]={0};
    int ret=1;
    while(true){
        ret=tc->recv(buf,sizeof(buf)-1);
        if(ret==0){//为0说明客户端断开连接
            //释放资源
            pthread_mutex_lock(&mutex);
            delete mp[client_addr];
            mp.erase(client_addr);
            pthread_mutex_unlock(&mutex);
            break;
        }
        buf[ret]='\0';
        string s=buf;
        printf("%s\n",buf);
        pthread_mutex_lock(&mutex);
        //将新收到的消息塞入队列，pair<地址信息，消息内容>
        msg.push({client_addr,s});
        //队列塞入了消息，向广播线程发送可以广播的信号
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}

int main(){
    tcpServer * ts=new tcpServer();
    ts->initServer();
    //创建广播线程
    pthread_t bc;
    pthread_create(&bc,nullptr,brodcast,nullptr);
    pthread_detach(bc);
    while(true){
        //接收客户端套接字，创建tcpChild对象用于对接
        tcpChild *tc=ts->start_accept();
        pthread_t tid;
        //开一个与新连接客户端通讯的进程
        int n=pthread_create(&tid,nullptr,recv_routine,tc);
        pthread_detach(tid);
    }
    delete ts;
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}