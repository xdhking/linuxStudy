#include<iostream>
#include<string>
#include<cstring>
#include<cstdio>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<cerrno>
#include<cassert>
#include<fcntl.h>
#define NAMED_PIPE_PATH "./mypipe"
bool createFifo(const char *path){
    umask(0);
    int n=mkfifo(path,0600);
    if(n==-1){
        std::cout<<"error is :"<<strerror(errno)<<std::endl;
        return false;
    }
    else
    return true;
}
void removeFifo(const char *path){
    int n=unlink(path);
    assert(n==0);
    (void)n;
}