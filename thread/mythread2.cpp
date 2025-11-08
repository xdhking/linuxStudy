#include<iostream>
#include<cstdio>
#include<unistd.h>
#include<pthread.h>
#include<cassert>
using namespace std;
//添加__thread , 可以将一个内置类型设置为线程局部存储
__thread int g_val=100;
string changeId(const pthread_t& id){
    char tid[64];
    snprintf(tid,sizeof(tid),"0x%x",id);
    return tid;
}

void *func_1(void *args){
    string s=static_cast<const char*>(args);
    int cnt=10;
    while(cnt--){
        cout<<s<<" is running"<<endl;
        cout<<"threadid : "<<changeId(pthread_self())<<endl;
        cout<<"thread &g_val: "<<&g_val<<" thread g_val"<<g_val++<<endl;
        sleep(1);
    }
    return nullptr;
}
int main(){
    pthread_t tid;
    pthread_create(&tid,nullptr,func_1,(void*)"thread 1");
    pthread_detach(tid);
    cout<<"main id :"<<changeId(pthread_self())<<\
        " main &g_val: "<<&g_val<<" main g_val: "<<g_val<<endl;
    while(true){
        sleep(1);
    }
    return 0;
}