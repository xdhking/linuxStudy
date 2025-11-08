#include "RingQueue.hpp"
#include "Task.hpp"
#include<iostream>
#include<unistd.h>
#include<cstdio>
#include<cstdlib>
#include<ctime>
#define NUM 5
std::string op="+-*/%";
std::string calc(int x,int y,char op){
    int ret=0;
    switch(op){
        case '+':ret=x+y;
        break;
        case '-':ret=x-y;
        break;
        case '*':ret=x*y;
        break;
        case '/':{
            if(y==0){
                return "div zero wrong";
            }
            else{
                ret=x/y;
            }
            break;
        }
        case '%':{
            if(y==0){
                return "mod zero wrong";
            }
            else{
                ret=x%y;
            }
            break;
        }
    }
    char buffer[64];
    snprintf(buffer,sizeof buffer,"%d %c %d = %d",x,op,y,ret);
    return buffer;
}
std::string getSelfName(){
    char buffer[256];
    snprintf(buffer,sizeof buffer,"thread[0x%x]",pthread_self());
    return buffer;
}
void *comsumer(void * args){
    RingQueue<Task> *rq=static_cast<RingQueue<Task>*>(args);
    while(true){
        sleep(1);
        Task t;
        rq->Pop(t);
        std::cout<<getSelfName()<<" : "<<t()<<std::endl;
    }
}
void *productor(void *args){
    RingQueue<Task> *rq=static_cast<RingQueue<Task>*>(args);
    while(true){
        int x=rand()%10000;
        int y=rand()%10000;
        int idx=rand()%5;
        rq->Push(Task(calc,x,y,op[idx]));
        std::cout<<getSelfName()<<" : "<<x<<' '<<op[idx]<<' '<<y<<" = ?"<<std::endl;
    }
}
int main(){
    srand((unsigned int)time(nullptr)^getpid());
    RingQueue<Task> *rq=new RingQueue<Task>;
    pthread_t c[NUM],p[NUM];
    for(int i=0;i<NUM;i++){
        pthread_create(&c[i],nullptr,comsumer,rq);
    }
    for(int i=0;i<NUM;i++){
        pthread_create(&p[i],nullptr,productor,rq);
    }
    for(int i=0;i<NUM;i++){
        pthread_join(c[i],nullptr);
    }
    for(int i=0;i<NUM;i++){
        pthread_join(p[i],nullptr);
    }
    delete rq;
    return 0;
}