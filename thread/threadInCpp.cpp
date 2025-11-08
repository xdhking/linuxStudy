#include<iostream>
#include<thread>
#include<unistd.h>
using namespace std;
void func_1(){
    int cnt=10;
    while(cnt--){
        cout<<"子线程：1"<<endl;
        sleep(1);
    }
}
int main(){
    thread t1(func_1);
    thread t2([&](){
        int cnt=10;
        while(cnt--){
            cout<<"子线程：2"<<endl;
            sleep(1);
        }
    });
    //任何语言，在linux中如果要实现多线程，必定要是用pthread库
    //c++的多线程，在linux环境中，本质是对pthread的库的封装
    int cnt=15;
    while(cnt--){
        cout<<"我是主线程"<<endl;
        sleep(1);
    }
    t1.join();
    t2.join();
    exit(0);
}