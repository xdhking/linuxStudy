#include<iostream>
#include<unistd.h>
#include<cassert>
#include<sys/wait.h>
#include<sys/types.h>
#include<string>
#include<cstdio>
#include<cstring>
using namespace std;
int main(){
    int fds[2];
    int n=pipe(fds);
    assert(n==0);
    cout<<"fds[0]: "<<fds[0]<<endl;
    cout<<"fds[1]: "<<fds[1]<<endl;
    pid_t id=fork();
    assert(id>=0);
    if(id==0){
        close(fds[0]);
        const char *s ="我是子进程，我正在给你发消息";
        int cnt=0;
        while(true){
            cnt++;
            char buffer[1024];
            snprintf(buffer,sizeof buffer,"child->parent say:%s[%d][%d]\n",s,cnt,getpid());
            write(fds[1],buffer,strlen(buffer));
            // sleep(1);
            // cout<<"count: "<<cnt<<endl;
            // break;
        }
        close(fds[1]);
        exit(0);
    }
    close(fds[1]);
    while(true){
        sleep(2);
        char buffer[1024];
        ssize_t s=read(fds[0],buffer,sizeof(buffer)-1);
        if(s>0){
            buffer[s]=0;
            cout<<"Get Message #"<<buffer<<"|mypid: "<<getpid()<<endl;
        }
        else if(s==0){
            cout<<"read: "<<s<<endl;
            break;
        }
        break;
    }
    close(fds[0]);
    cout<<"父进程关闭读端"<<endl;
    int status=0;
    n=waitpid(id,&status,0);
    assert(n==id);
    cout<<"pid->"<<n<<" : "<<(status&0x7f)<<endl;
}