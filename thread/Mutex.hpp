#pragma once
#include<pthread.h>
class Mutex{
public:
    Mutex(const pthread_mutex_t &mtx):mtx_(mtx){
        pthread_mutex_lock(&mtx_);
    }
    ~Mutex(){
        pthread_mutex_unlock(&mtx_);
    }
private:
    pthread_mutex_t mtx_;
};