#include "comm.hpp"
int main(){
    if(!createFifo(NAMED_PIPE_PATH)) exit(1);
    std::cout<<"server begin"<<std::endl;
    int rfd=open(NAMED_PIPE_PATH,O_RDONLY);
    std::cout<<"debug: "<<rfd<<std::endl;
    assert(rfd>0);
    std::cout<<"server end"<<std::endl;
    char buffer[1024];
    while(true){
        ssize_t n=read(rfd,buffer,sizeof(buffer));
        if(n==0){
            break;
        }
        assert(n>0);
        std::cout<<"client->server: "<<buffer<<std::endl;
    }
    close(rfd);
    removeFifo(NAMED_PIPE_PATH);
    return 0;
}