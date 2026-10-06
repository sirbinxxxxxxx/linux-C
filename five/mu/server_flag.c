#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "send_all.h"
#include "recv_line.h"

volatile sig_atomic_t call = 0;
void handler(int sig)
{
    (void)sig;
    call =1 ;
}

int main()
{
    //注册信号函数
    struct sigaction sa = {0};
    sa.sa_handler =handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags =0;

    if(sigaction(SIGCHLD,&sa,NULL)==-1){
        perror("sigaction");
        return 1;
    }

    int socketfd = socket(AF_INET,
        SOCK_STREAM,
        0
    );
    if(socketfd == -1){
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family=AF_INET;
    addr.sin_port = htons(5000);
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET,
        "127.0.0.1",
        &addr.sin_addr.s_addr
    );

    //unlink(&addr.sin_addr.s_addr);

    if(bind(socketfd,
            (const struct sockaddr *)&addr,
            sizeof(addr)) == -1
        ){
            perror("bind");
            close(socketfd);
            return 1;
        }

    printf("before listen\n");
    if(listen(socketfd,128)  == -1){
        perror("listen");
        return 1;
    }
    printf("after listen\n");

    while(1){
        if(call){
            call = 0;
            int status;
            pid_t child;
            while ((child =  waitpid(-1,&status,WNOHANG)) > 0)
            {
                printf("回收子进程%ld\n",(long)child);
            }
            
        }

    struct sockaddr_in client_addr = {0};
    socklen_t client_len =sizeof(client_addr) ;
    

    printf("before accept\n\n");
    int client_fd = accept(socketfd,(struct sockaddr *)&client_addr,&client_len);
    if(client_fd == -1){
        if(errno == EINTR){
            continue;
        }
        perror("accept");
        return 1;
    }
    printf("after accept\n\n");    
    uint16_t port = ntohs(client_addr.sin_port);
    char buf[INET_ADDRSTRLEN];
    if(inet_ntop(client_addr.sin_family,
        &client_addr.sin_addr,
        buf,
        (socklen_t)sizeof(buf)
    ) == NULL){
        fprintf(stderr,"inet_ntop unsuccessed\n");
        close(socketfd);
        close(client_fd);
        return 1;
    }
    printf("[contect] ip:port %s:%d\n",buf,port);

    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        close(socketfd);
        close(client_fd);
        return 1;
    } else if (pid == 0){
        close(socketfd);
        char buf[256];
        size_t size = sizeof(buf);
        while(1){
            ssize_t n ;
            n= recv_line(client_fd,buf,size);
            if( n <0){
                fprintf(stderr,"recv_all unsuccessed");
                break;
            }else if(n>0){
                printf("服务器：\nrecv_line() 收到完整一行\nn=%d\n", (int)n);
                fflush(stdout);    
            }else{
                printf("客户端关闭连接\n");
                break;
            }

            

            if(send_all(client_fd,buf,(size_t)n)== -1){
                fprintf(stderr,"send_all unsuccessed");
               break;
            } 
            printf("服务器：\nsend_all(...,%d)\n",(int)n);
            fflush(stdout);
        }
        close(client_fd);
        _exit(0);
    
    }else{
        close(client_fd);
    }
    }
  
    close(socketfd);
    return 0;
}