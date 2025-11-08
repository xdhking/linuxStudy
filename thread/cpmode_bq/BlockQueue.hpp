#pragma once
#include<pthread.h>
#include<cassert>
#include<queue>
const int maxsize=5;
template<class T>
class BlockQueue{
public:
    BlockQueue(){
        int n;
        n=pthread_mutex_init(&m_mutex,nullptr);
        assert(n==0);
        n=pthread_cond_init(&m_cpond,nullptr);
        assert(n==0);
        n=pthread_cond_init(&m_ppond,nullptr);
        assert(n==0);
        (void)n;
    }
    ~BlockQueue(){
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cpond);
        pthread_cond_destroy(&m_ppond);
    }
    void push(const T& task){
        pthread_mutex_lock(&m_mutex);
        while(is_full()){
            pthread_cond_wait(&m_ppond,&m_mutex);
        }
        m_q.push(task);
        pthread_cond_signal(&m_cpond);
        pthread_mutex_unlock(&m_mutex);
    }

    void pop(T &task){
        pthread_mutex_lock(&m_mutex);
        while(is_empty()){
            pthread_cond_wait(&m_cpond,&m_mutex);
        }
        task=m_q.front();
        m_q.pop();
        pthread_cond_signal(&m_ppond);
        pthread_mutex_unlock(&m_mutex);
    }
private:
    bool is_empty(){
        return m_q.size()==0;
    }
    bool is_full(){
        return m_q.size()==maxsize;
    }
private:
    std::queue<T> m_q;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cpond,m_ppond;
};