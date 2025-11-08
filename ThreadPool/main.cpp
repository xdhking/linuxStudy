#include "ThreadPool.hpp"
#include "Task.hpp"
#include<memory>
#include<iostream>
#include<unistd.h>

int main(){
    std::unique_ptr<ThreadPool<Task>> tp(new ThreadPool<Task>);
    tp->run();
    while(true){
        int x,y;
        char op;
        std::cout<<"请输入第一个数字# ";
        std::cin>>x;
        std::cout<<"请输入第二个数字# ";
        std::cin>>y;
        std::cout<<"请输入运算符号# ";
        std::cin>>op;
        tp->Push(Task(x,y,op));
        sleep(1);
    }
    return 0;
}