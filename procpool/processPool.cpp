#include<iostream>
#include<cstdio>
#include<unistd.h>
#include<cassert>
#include<sys/types.h>
#include<vector>
#include<ctime>
#include<sys/wait.h>
#define PROC_NUM 5
#define TASK_NUM 3
#define makeSeed() srand((unsigned long)time(nullptr)^getpid()^0x17123^rand()%1234)
typedef void (*func_t)();
int runtimes;
class subProc{
public:
    std::string m_name;
    pid_t m_pid;
    int m_wfd;
    subProc(const pid_t &id,const int &fd):m_pid(id),m_wfd(fd){
        char buffer[1024];
        snprintf(buffer,sizeof(buffer)-1,"child process pid[%d],fd[%d]",id,fd);
        m_name=buffer;
    }
};
void downLoadTask(){
    std::cout<<getpid()<<" 下载任务"<<std::endl;
}
void ioTask(){
    std::cout<<getpid()<<" IO任务"<<std::endl;
}
void flushTask(){
    std::cout<<getpid()<<" 刷新任务"<<std::endl;
}
void runTask(int fd,std::vector<func_t> *task){
    while(true){
        int Tid=0;
        size_t n=read(fd,&Tid,sizeof(int));
        if(n==0){
            exit(0);
        }
        assert(n==4);
        (void)n;
        std::cout<<"子进程"<<getpid()<<"已收到"<<std::endl;
        (*task)[Tid]();
    }
}
void createProcess(std::vector<subProc> *sp,std::vector<func_t> *task){
    std::vector<int> deleteFD;
    for(int i=0;i<PROC_NUM;i++){
        int fds[2];
        int n=pipe(fds);
        assert(n==0);
        (void)n;
        pid_t id=fork();
        assert(id>=0);
        if(id==0){
            for(int j=0;j<(int)deleteFD.size();j++){
                close(deleteFD[j]);
            }
            close(fds[1]);
            runTask(fds[0],task);
            exit(0);
        }
        close(fds[0]);
        subProc sub(id,fds[1]);
        sp->push_back(std::move(sub));
        deleteFD.push_back(fds[1]);
    }
}

void loadTask(std::vector<func_t> *task){
    task->push_back(downLoadTask);
    task->push_back(ioTask);
    task->push_back(flushTask);
}
void sendTask(std::vector<subProc> &sp){
    while(runtimes--){
        int subPid=rand()%PROC_NUM;
        int Taskid=rand()%TASK_NUM;
        fprintf(stdout,"父进程向%s发送了任务[%d]\n",sp[subPid].m_name.data(),Taskid);
        int n=write(sp[subPid].m_wfd,&Taskid,sizeof(int));
        assert(n==sizeof(int));
        (void)n;
        sleep(2);
    }
}
void closeProcFD(std::vector<subProc> &sp){
    for(int i=0;i<PROC_NUM;i++){
        close(sp[i].m_wfd);
    }
    std::cout<<"所有子进程fd已关闭"<<std::endl;
}
void waitProc(std::vector<subProc> &sp){
    for(int i=0;i<PROC_NUM;i++){
        pid_t n=waitpid(sp[i].m_pid,nullptr,0);
        assert(n>=0);
    }
    std::cout<<"wait child process successfully"<<std::endl;
} 
int main(){
    makeSeed();
    std::cout<<"输入任务执行次数，输入-1执行无限次"<<std::endl;
    std::cin>>runtimes;
    std::vector<subProc> sp;
    std::vector<func_t> task;
    loadTask(&task);
    createProcess(&sp,&task);
    sendTask(sp);
    closeProcFD(sp);
    waitProc(sp);
    return 0;
}