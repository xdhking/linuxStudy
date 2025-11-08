#include "comm.hpp"
int main(){
    std::cout<<"client begin"<<std::endl;
    int wfd=open(NAMED_PIPE_PATH,O_WRONLY);
    assert(wfd>0);
    std::cout<<"please say#"<<std::endl;
    char buffer[1024];
    while(true){
        char *s=fgets(buffer,sizeof(buffer),stdin);
        if(s==NULL){
            break;
        }
        buffer[strlen(buffer)-1]='\0';
        ssize_t n=write(wfd,buffer,strlen(buffer)+1);
        assert(n>0);
        (void)n;
    }
    close(wfd);
    std::cout<<"over"<<std::endl;
    return 0;
}