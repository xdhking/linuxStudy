#include<iostream>
#include<vector>
#include<cstdio>
#include<unistd.h>
#include<cassert>
#include "Mutex.hpp"
int tickets=10000;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
class ThreadData{
public:
    pthread_t m_tid;
    char m_buffer[64];
};
void *getTicket(void *args){
    ThreadData* td=static_cast<ThreadData*>(args);
    long long getnum=0;
    while(true){
        {
            Mutex m(mutex);
            usleep(1254);
            if(tickets>0){
                std::cout<<td->m_buffer<<" : 还剩票数 "<<tickets<<std::endl;
                tickets--;
                getnum++;
            }
            else{
                break;
            }
        }
    }
    return (void*)getnum;
}

int main(){
    std::vector<ThreadData*> tds;
#define NUM 10
    for(int i=1;i<=NUM;i++){
        ThreadData *td=new ThreadData;
        snprintf(td->m_buffer,sizeof(td->m_buffer),"thread[%d]",i);
        int n=pthread_create(&td->m_tid,nullptr,getTicket,(void*)td);
        assert(n==0);
        (void)n;
        tds.push_back(std::move(td));
    }
    for(int i=0;i<(int)tds.size();i++){
        long long ret;
        int n=pthread_join(tds[i]->m_tid,(void**)&ret);
        assert(n==0);
        (void)n;
        std::cout<<tds[i]->m_buffer<<"获得票数："<<ret<<std::endl;
        delete tds[i];
    }

}