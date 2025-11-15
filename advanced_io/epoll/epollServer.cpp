#include "epollServer.hpp"
#include "log.hpp"
#include<iostream>
#include<memory>
using namespace std;

int main(int argc,char *argv[]){
    if(argc!=2){
        logMessage(FATAL,"the number of arguments is error");
        return 0;
    }
    unique_ptr<epollServer> epsvr(new epollServer());
    epsvr->initServer();
    epsvr->start();
    return 0;
}