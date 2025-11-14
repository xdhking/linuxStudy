#include "log.hpp"
#include "sockServer.hpp"
#include<iostream>
#include<cstdlib>
#include<memory>
using namespace std;

int routine(int sock,int pos){

}

int main(int argc,char *argv[]){
    if(argc!=2){
        logMessage(ERROR,"number of argument is error");
        return 0;
    }
    uint16_t port=atoi(argv[1]);
    unique_ptr<server> sv(new server(port));
    sv->init();
    sv->start(routine);
    return 0;
}