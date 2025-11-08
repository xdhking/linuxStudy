#pragma once
#include<pthread.h>
#include<functional>
#include<cassert>
#include<cstdio>
#include<unordered_map>
namespace ThreadNs
{

class Thread{
    using func_t=std::function<void*(void*)>;
public:
    Thread(){
        id++;
        char buffer[64];
        snprintf(buffer,sizeof(buffer),"thread-%d",id);
        name=buffer;
    }
    ~Thread(){
        if(_tid){
            // pthread_detach(_tid);
            pthread_cancel(_tid);
            mp[_tid]=nullptr;
        }
    }
    static void* start_rutine(void* args){
        Thread *_this=static_cast<Thread*>(args);
        return _this->_fun(_this->_args);
    }
    void start(func_t fun,void *args){
        _fun=fun;
        _args=args;
        int n=pthread_create(&_tid,nullptr,start_rutine,this);
        assert(n==0);
        (void)n;
        mp[_tid]=this;
    }
    void join(){
        pthread_join(_tid,nullptr);
        mp[_tid]=nullptr;
        _tid=0;
    }
    void join(void **ret){
        pthread_join(_tid,ret);
       mp[_tid]=nullptr;
        _tid=0;
    }
    static Thread* getself(const pthread_t &tid){
        return mp[tid];
    }
private:
    pthread_t _tid;
    func_t _fun;
    void *_args;
    static unsigned long id;
    static std::unordered_map<pthread_t,Thread*> mp;
public:
    std::string name;
};
unsigned long Thread::id=0;
std::unordered_map<pthread_t,Thread*> Thread::mp;
}