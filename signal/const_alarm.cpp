#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
using namespace std;
bool flag;
int cnt=0;
void catchSignal(int num){
    cout<<"alarmed "<<endl;
    if(flag){
        alarm(0);
        return;
    }
    else{
        cout<<++cnt<<" times "<<endl;
        alarm(10);
        // exit(1);
        return;
    }
}
int main(){
    signal(SIGALRM,catchSignal);
    cin>>flag;
    alarm(10);
    cout<<getpid()<<endl;
    while(true){
        cout<<"running...."<<endl;
        sleep(1);
    }
    return 0;
}