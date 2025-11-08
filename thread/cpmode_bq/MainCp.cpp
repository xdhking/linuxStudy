#include "BlockQueue.hpp"
#include "Task.hpp"
#include<ctime>
#include<unistd.h>
#include<iostream>
using func_t=std::function<int(int,int)>;
std::vector<func_t> fs;
int add(int x,int y){
    return x+y;
}

int divid(int x,int y){
    return x/y;
}
int mul(int x,int y){
    return x*y;
}
void *comsumer(void *args){
    BlockQueue<Task> *bq=static_cast<BlockQueue<Task>*>(args);
    while(true){
        Task t;
        bq->pop(t);
        std::cout<<"消费数据："<<t()<<std::endl;
        sleep(1);
    }
}
void *productor(void *args){
    BlockQueue<Task> *bq=static_cast<BlockQueue<Task>*>(args);
    while(true){
        int x=rand()%10+1;
        int y=rand()%10+1;
        int taskid=rand()%3;
        bq->push(Task(x,y,fs[taskid]));
        std::cout<<"生产数据："<<x<<' '<<y<<' '<<taskid<<std::endl;
        sleep(1);
    };
}
void init(){
    fs.push_back(add);
    fs.push_back(divid);
    fs.push_back(mul);
}
int main(){
    init();
    srand((unsigned int)time(nullptr)^getpid());
    BlockQueue<Task> *bq=new BlockQueue<Task>();
    pthread_t c,p;
    pthread_create(&c,nullptr,comsumer,(void*)bq);
    pthread_create(&p,nullptr,productor,(void*)bq);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);
}