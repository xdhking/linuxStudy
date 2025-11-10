#pragma once
#include "LockGuard.hpp"
#include<pthread.h>
#include<queue>
#include<cassert>
#define MAXSIZE 10
template<class T>
class BlockQueue{
private:
    bool is_empty(){
        return _q.size()==0;
    }
    bool is_full(){
        return _q.size()==_sz;
    }
public:
    BlockQueue(const int &sz=MAXSIZE):_sz(sz){
        int n=pthread_mutex_init(&_mutex,nullptr);
        assert(n==0);
        n=pthread_cond_init(&_cpond,nullptr);
        assert(n==0);
        n=pthread_cond_init(&_ppond,nullptr);
        assert(n==0);
        (void)n;
    }
    void Push(const T &task){
        {
            LockGuard lg(&_mutex);
            while(is_full()){
                pthread_cond_wait(&_ppond,&_mutex);
            }
            _q.push(task);
        }
        pthread_cond_signal(&_cpond);
    }
    void Pop(T &task){
        {
            LockGuard lg(&_mutex);
            while(is_empty()){
                pthread_cond_wait(&_cpond,&_mutex);
            }
            task=_q.front();
            _q.pop();
        }
        pthread_cond_signal(&_ppond);
    }
    ~BlockQueue(){
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cpond);
        pthread_cond_destroy(&_ppond);
    }
private:
    int _sz;
    std::queue<T> _q;
    pthread_mutex_t _mutex;
    pthread_cond_t _cpond;
    pthread_cond_t _ppond;
};