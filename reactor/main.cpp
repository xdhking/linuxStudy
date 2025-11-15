#include "log.hpp"
#include "reactorServer.hpp"
#include<memory>
using namespace std;
int main(int argc,char *argv[]){
    if(argc!=2){
        logMessage(ERROR,"argument error");
        exit(0);
    }
    uint16_t port=atoi(argv[1]);
    unique_ptr<Server> svr(new Server());
    svr->initServer();
    svr->start();
    return 0;
}