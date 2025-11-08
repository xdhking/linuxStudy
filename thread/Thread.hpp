#pragma once
#include<iostream>
#include<string>
#include<cstring>
#include<functional>
#include<unistd.h>
#include<cassert>
#include<pthread.h>
class Thread;
//上下文
class Context{
public:
    Thread *this_;
    void *args_;
public:
    Context():this_(nullptr),args_(nullptr){}
    ~Context(){}
};
class Thread{
    //using func_t = std::function<void*(void*)>;
    typedef std::function<void*(void*)> func_t;
    const int num=1024;
public:
    Thread(func_t func,void *args,int number):func_(func),args_(args){
       char buffer[num];
       snprintf(buffer,sizeof buffer,"thread-%d",number);
       name_=buffer; 
    }
    //在类内创建线程，想让线程执行对应的方法，需要将方法设置为static
    static void *start_routine(void *args){//类内成员，有缺省参数
        //静态方法不能调用非静态成员方法或者成员变量
        // return func_(args_);
        Context *ctx=static_cast<Context*>(args);
        void *ret=ctx->this_->run(ctx->args_);
        delete ctx;
        return ret;
    }

    void start(){
        Context *ctx=new Context();
        ctx->this_=this;
        ctx->args_=args_;
        int n=pthread_create(&tid_,nullptr,start_routine,ctx);
        assert(n==0);
        (void)n;
    }

    void join(){
        int n=pthread_join(tid_,nullptr);
        assert(n==0);
        (void)n;
    }

    void *run(void *args){
        return func_(args);
    }
    ~Thread(){
        //do nothing
    }
private:
    std::string name_;
    func_t func_;
    void *args_;

    pthread_t tid_;
};