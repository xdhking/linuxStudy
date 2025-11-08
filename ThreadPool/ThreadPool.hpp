#pragma once
#include "Thread.hpp"
#include "LockGuard.hpp"
#include<vector>
#include<queue>
#include<pthread.h>
#include<semaphore.h>
#include<iostream>
using namespace ThreadNs;
const int gcap=100,gnum=10;
template<class T>
class ThreadPool{
    using func_p=void*(*)(void*);
private:
    static void* rutine(void *args){
        ThreadPool<T> *tp=static_cast<ThreadPool<T>*>(args);
        while(!tp->is_exit){
            T task;
            tp->Pop(task);
            std::cout<<Thread::getself(pthread_self())->name<<" : ";
            task();
        }
        return nullptr;
    }
    bool is_full(){
        return _q.size()==_cap;
    }
    bool is_empty(){
        return _q.size()==0;
    }
public:
    ThreadPool(const int &cap=gcap,const int &num=gnum):_cap(cap),_num(num){
        for(int i=0;i<_num;i++){
            _tds.push_back(new Thread);
        }
        int n=pthread_mutex_init(&_mutex,nullptr);
        assert(n==0);
        n=pthread_cond_init(&_condPush,nullptr);
        assert(n==0);
        n=pthread_cond_init(&_condPop,nullptr);
        assert(n==0);
        (void)n;
        is_exit=false;
    }
    ~ThreadPool(){
        for(Thread *td:_tds){
            delete td;
        }
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_condPush);
        pthread_cond_destroy(&_condPop);
    }
    void run(){
        for(int i=0;i<_num;i++){
            _tds[i]->start(rutine,this);
        }
    }
    void run(func_p fun){
        for(int i=0;i<_num;i++){
            _tds[i]->start(fun,this);
        }
    }
    void wait(){
        is_exit=true;
        for(int i=0;i<_num;i++){
            _tds[i]->join();
        }
    }
    void wait(void **ret){
        for(int i=0;i<_num;i++){
            _tds[i]->join(ret+i);
        }
    }
    void Push(const T &task){
        LockGuard lockguard(&_mutex);
        while(is_full()){
            pthread_cond_wait(&_condPush,&_mutex);
        }
        _q.push(task);
        pthread_cond_signal(&_condPop);
    }
    void Pop(T &task){
        LockGuard lockguard(&_mutex);
        while(is_empty()){
            pthread_cond_wait(&_condPop,&_mutex);
        }
        task=_q.front();
        _q.pop();
        pthread_cond_signal(&_condPush);
    }
private:
    std::vector<Thread*> _tds;
    pthread_mutex_t _mutex;
    pthread_cond_t _condPush,_condPop;
    std::queue<T> _q;
    int _cap,_num;
public:
    bool is_exit;
};