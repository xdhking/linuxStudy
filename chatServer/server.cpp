#include<iostream>
#include<cstdio>
#include<cstring>
#include<unistd.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<cstdlib>
#include<vector> 
using namespace std;

vector<int> client_fds;

void* task(void * arg) {
    int client_fd = (int)(long)arg;
    char nickname[128] = {0};
    char recv_buf[1024] = {0};
    ssize_t recv_len = recv(client_fd, nickname, sizeof(nickname)-1, 0);
    if (recv_len <= 0) { 
        close(client_fd);
        return nullptr;
    }
    nickname[recv_len] = '\0';
    client_fds.push_back(client_fd);

    char join_msg[256] = {0};
    snprintf(join_msg, sizeof(join_msg), "[系统]# %s 加入了聊天室！", nickname);
    for (int fd : client_fds) {
        if (fd != client_fd) {
            send(fd, join_msg, strlen(join_msg), 0);
        }
    }

    while (true) {
        memset(recv_buf, 0, sizeof(recv_buf));
        recv_len = recv(client_fd, recv_buf, sizeof(recv_buf)-1, 0);
        if (recv_len <= 0) {
            for (auto it = client_fds.begin(); it != client_fds.end(); ++it) {
                if (*it == client_fd) {
                    client_fds.erase(it);
                    break;
                }
            }
            char leave_msg[256] = {0};
            snprintf(leave_msg, sizeof(leave_msg), "[系统]# %s 离开了聊天室！", nickname);
            for (int fd : client_fds) {
                send(fd, leave_msg, strlen(leave_msg), 0);
            }
            close(client_fd);
            break;
        }

        char broadcast_msg[1024] = {0};
        snprintf(broadcast_msg, sizeof(broadcast_msg), "[%s]# %s", nickname, recv_buf);
        for (int fd : client_fds) {
            if (fd != client_fd) {
                send(fd, broadcast_msg, strlen(broadcast_msg), 0);
            }
        }
    }

    return nullptr;
}

int main(){
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket create failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(8080); 
    local.sin_addr.s_addr = htonl(INADDR_ANY); 
    if (bind(listenfd, (struct sockaddr*)&local, sizeof(local)) < 0) {
        perror("bind failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 5) < 0) {
        perror("listen failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }
    printf("服务端启动成功，监听端口 8080\n");

    while(true) {
        struct sockaddr_in src;
        socklen_t len = sizeof(src);
        int client_fd = accept(listenfd, (struct sockaddr*)(&src), &len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        printf("客户端[%s:%d] 连接成功\n", inet_ntoa(src.sin_addr), ntohs(src.sin_port));
        pthread_t tid;
        // 注意：int转void*需转long，避免64位系统截断
        int n = pthread_create(&tid, nullptr, task, (void*)(long)client_fd);
        if (n != 0) {
            perror("pthread_create failed");
            close(client_fd);
            continue;
        }
        pthread_detach(tid);
    }

    close(listenfd);
    return 0;
}