#include<semaphore.h>
#include<pthread.h>
#include<vector>
const int gcap=5;
template<class T>
class RingQueue{
public:
    RingQueue(const int &cap=gcap):_cap(cap){
        _q.resize(_cap);
        sem_init(&psem,0,_cap);
        sem_init(&csem,0,0);
        pthread_mutex_init(&pmutex,nullptr);
        pthread_mutex_init(&cmutex,nullptr);
        pid=cid=0;
    }
    ~RingQueue(){
        sem_destroy(&psem);
        sem_destroy(&csem);
        pthread_mutex_destroy(&pmutex);
        pthread_mutex_destroy(&cmutex);
    }
    void Push(const T& task){
        P(psem);
        pthread_mutex_lock(&pmutex);
        _q[pid++]=task;
        pid%=_cap;
        pthread_mutex_unlock(&pmutex);
        V(csem);
    }
    void Pop(T& task){
        P(csem);
        pthread_mutex_lock(&cmutex);
        task=_q[cid++];
        cid%=_cap;
        pthread_mutex_unlock(&cmutex);
        V(psem);
    }
private:
    void P(sem_t &sem){
        sem_wait(&sem);
    }
    void V(sem_t &sem){
        sem_post(&sem);
    }
    std::vector<T> _q;
    sem_t psem,csem;
    pthread_mutex_t pmutex,cmutex;
    int pid,cid;
    int _cap;
};