#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<cstdlib>
#include<cerrno>
#include<cstring>
using namespace std;
int main(int argc,char *argv[]){
    if(argc!=3){
        cout<<"format is wrang"<<endl;
        exit(1);
    }
    int procid=atoi(argv[1]);
    int sig=atoi(argv[2]);
    int ret=kill(procid,sig);
    if(ret<0){
        cerr<<errno<<" : "<<strerror(errno)<<endl;
    }
    return 0;
}
