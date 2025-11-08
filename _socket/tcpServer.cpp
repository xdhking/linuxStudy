#include "network.hpp"
#include<pthread.h>
#include<queue>
#include<string>
#include<unordered_map>
#include<cstring>
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
            pthread_cond_wait(&cond,&mutex);
        }
        pair<string,string> tmp=msg.front();
        msg.pop();
        char buf[1024]={0};
        snprintf(buf,sizeof(buf)-1,"%s: %s",tmp.first,tmp.second);
        for(auto &client:mp){
            string key=client.first;
            tcpChild *tc=client.second;
            if(key==tmp.first)
                continue;
            tc->send(buf,strlen(buf));
        }
        pthread_mutex_unlock(&mutex);
    }
}

void *recv_routine(void *args){
    tcpChild *tc=static_cast<tcpChild*>(args);
    string client_addr="["+tc->get_ip()+"]"+"["+to_string(tc->get_port())+"]";
    pthread_mutex_lock(&mutex);
    mp[client_addr]=tc;
    pthread_mutex_unlock(&mutex);
    char buf[1024]={0};
    int ret=1;
    while(true){
        ret=tc->recv(buf,sizeof(buf));
        if(ret==0){
            pthread_mutex_lock(&mutex);
            mp.erase(client_addr);
            pthread_mutex_unlock(&mutex);
            break;
        }
        string s=buf;
        pthread_mutex_lock(&mutex);
        msg.push({client_addr,s});
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    delete tc;
    return nullptr;
}

int main(){
    tcpServer * ts=new tcpServer();
    ts->initServer();
    pthread_t bc;
    pthread_create(&bc,nullptr,brodcast,nullptr);
    pthread_detach(bc);
    while(true){
        tcpChild *tc=new tcpChild(ts->start_accept());
        pthread_t tid;
        int n=pthread_create(&tid,nullptr,recv_routine,tc);
        pthread_detach(tid);
    }
    delete ts;
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}