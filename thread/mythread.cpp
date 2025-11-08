#include<iostream>
#include<unistd.h>
#include<cassert>
#include<vector>
#include<pthread.h>
using namespace std;
//新线程
void *thread_routine(void *args){
    string name = static_cast<const char*>(args);
    while(true){
        cout<<"my name is "<<name<<endl;
        // cout<<"I`m a new thread,i`m running,my name is "<<name<<endl;
        sleep(1);
    }
}
//解决方案
class ThreadData{
public:
    pthread_t tid;
    char namebuffer[64];
};
void *func_2(void *args){
    ThreadData *td=static_cast<ThreadData*>(args);
    int cnt=10;
    while(cnt--){
        cout<<"my name is "<<td->namebuffer<<endl;
        sleep(1);
    }
    delete td;
    return nullptr;//return 的时候线程终止  or pthread_exit
    pthread_exit(nullptr);
}
class ThreadExit{
public:
    int exit_code;
    int exit_result;
};
void *func_3(void *args){
    ThreadData *td=static_cast<ThreadData*>(args);
    int cnt=10;
    while(cnt--){
        cout<<"my name is "<<td->namebuffer<<endl;
        sleep(1);
    }
    //ThreadExit td={1,1};,不能创建局部变量，线程函数退出时栈区会释放
    ThreadExit *res=new ThreadExit;
    res->exit_code=1;
    res->exit_result=1;
    pthread_exit(res);
    // return res;
}
int main(){
    vector<ThreadData*> threads;
    #define NUM 10
    for(int i=0;i<NUM;i++){
        ThreadData *td=new ThreadData;
        snprintf(td->namebuffer,sizeof(td->namebuffer),"child thread : %d",i+1);
        int n=pthread_create(&td->tid,nullptr,func_3,td);
        assert(n==0);
        threads.push_back(td);
    }
    //被取消的线程退出码为-1
    for(int i=0;i<(int)threads.size();i++){
        pthread_cancel(threads[i]->tid);
        int ret;
        int n=pthread_join(threads[i]->tid,(void**)&ret);
        assert(n==0);
        (void)n;
        cout<<"exit code: "<<ret<<endl;
    }
    exit(0);
    //waiting for child thread
    for(auto &pt:threads){
        ThreadExit *ret;
        int n=pthread_join(pt->tid,(void**)&ret);
        assert(n==0);
        (void)n;
        cout<<pt->namebuffer<<" quit successfully, "<<"exit code is "<<ret->exit_code<<" , exit result is "<<ret->exit_result<<endl;
        delete pt;
    }
    cout<<"all child thread have quited"<<endl;
    /*
    //解决方案
    vector<pthread_t> tids;
    #define NUM 10
    for(int i=0;i<NUM;i++){
        ThreadData *td=new ThreadData;
        snprintf(td->namebuffer,sizeof(td->namebuffer),"chile thread [%d]",i);
        pthread_create(&(td->tid),nullptr,func_2,(void*)td);
        tids.push_back(td->tid);
    }
    //waiting thread quit
    for(int i=0;i<(int)tids.size();i++){
        //阻塞式等待
        pthread_join(tids[i],nullptr);
    }
    cout<<"all thread quit successfully"<<endl;
    while(true){
        sleep(5);
    }
    */


    /*
    //1.创建一批线程
    vector<pthread_t> tids;
    #define NUM 10
    for(int i=0;i<NUM;i++){
        pthread_t tid;
        //创建第一个线程时，该线程来不及保存namebuffer的数据就切换到主线程
        //然后主线程修改namebuffer准备创建第二个线程，而导致前一个线程还未
        //保存的数据被第二个线程的数据所覆盖了
        char namebuffer[64];
        snprintf(namebuffer,sizeof(namebuffer),"chile thread [%d]",i);
        pthread_create(&tid,nullptr,thread_routine,(void*)namebuffer);
        tids.push_back(tid);
    }
    while(true){
        // cout<<"main thread is running"<<endl;
        sleep(5);
    }
    */

   /*
    pthread_t tid;
    int n=pthread_create(&tid,nullptr,thread_routine,(void *)"thread one");
    assert(0==n);
    (void)n;
    //主线程
    while(true){
        cout<<"I`m main thread"<<endl;
        sleep(1);
    }
    */
    return 0;
}