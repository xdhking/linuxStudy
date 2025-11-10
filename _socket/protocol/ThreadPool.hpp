#pragma once
#include<pthread.h>
#include<cassert>
#include "BlockQueue.hpp"
#include "Thread.hpp"
#define NUM 10
template<class T>
class ThreadPool{
private:
    static void* start_routine(void *args){
        ThreadPool<T> * tp=static_cast<ThreadPool<T>*>(args);
        while(true){
            T task;
            tp->Pop(task);
            task();
        }
    }
public:
    ThreadPool(const int &tdnum=NUM,const int &q_size=MAXSIZE){
        _bq=new BlockQueue<T>(q_size);
        for(int i=0;i<tdnum;i++){
            _tids.push_back(new Thread(start_routine,this));
        }
    }
    void operator=(const ThreadPool &)=delete;
    ThreadPool(const ThreadPool &)=delete;
    static ThreadPool<T> *getInstance(const int &tdnum=NUM,const int &q_size=MAXSIZE){
        if(nullptr==tp){
            tp=new ThreadPool<T>(tdnum,q_size);
        }
        return tp;
    }
    void Push(const T &task){
        _bq->Push(task);
    }
    void Pop(T &task){
        _bq->Pop(task);
    }
    ~ThreadPool(){
        for(int i=0;i<_tids.size();i++){
            delete _tids[i];
        }
        delete _bq;
    }

private:
    static BlockQueue<T> *_bq;
    std::vector<Thread*> _tids;
    static ThreadPool<T> *tp;
};
template<class T>
ThreadPool<T> *ThreadPool<T>::tp=nullptr;
template<class T>
BlockQueue<T> *ThreadPool<T>::_bq=nullptr;