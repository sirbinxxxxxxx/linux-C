#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
int main()
{
    int socketfd = socket(AF_INET,
        SOCK_STREAM,
        0
    );
    if(socketfd == -1){
        perror("secket");
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
    printf("before accept\n");
    int client_fd = accept(socketfd,NULL,NULL);
    if(client_fd == -1){
        perror("accept");
        return 1;
    }
    printf("after accept\n");

    char buf[256];
    ssize_t n;
    while(1){
    n = recv(client_fd,buf,sizeof(buf)-1,0);
    if( n < 0){
        perror("recv");
        close(client_fd);
        close(socketfd);
        return 1;
    }
    else if( n == 0){
        printf("客户端关闭连接\n");
        close(client_fd);
        break;
    }else{
        buf[n] = '\0';
        printf("%s\n",buf);
        fflush(stdout);
        //发送回显
      
        size_t total = 0;
        while((size_t)n >total){
        ssize_t n_w=send(client_fd,buf+total,(size_t)n-total , 0);
        if(n_w < 0){
            perror("send");
            close(client_fd);
            close(socketfd);
            return 1;
        }else if(n_w == 0 ){
            fprintf(stderr,"写入失败\n");
            close(client_fd);
            close(socketfd);
            return 1;
        }else{
            total+=n_w;
        }
        }

        
    }
    }
      
    }
  
    close(socketfd);
    return 0;
}