#pragma once
#include<functional>
#include<pthread.h>
#include<cassert>
class Thread{
    using func_t=std::function<void*(void*)>;
private:
    static void *start_routine(void *args){
        Thread *td=static_cast<Thread*>(args);
        return td->run();
    }
public:
    Thread(func_t fun,void *args):_fun(fun),_args(args){
        int n=pthread_create(&_tid,nullptr,start_routine,this);
        assert(n==0);
        (void)n;
    }
    void *run(){
        return _fun(_args);
    }
    void join(){
        pthread_join(_tid,nullptr);
    }
    void detach(){
        pthread_detach(_tid);
    }
    ~Thread(){
        detach();
    }
private:
    func_t _fun;
    void * _args;
    pthread_t _tid;
};