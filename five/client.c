#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
int main()
{
    int fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );
    if(fd==-1){
        perror("socket");
        return 1;

    }

    struct  sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    inet_pton(AF_INET,
        "127.0.0.1",
        &addr.sin_addr.s_addr
    );

    if(connect(fd,
        (const struct sockaddr*)&addr,
        sizeof(addr)
    ) == -1 
    ){
        perror("connect");
        return 1;
    }

    char buf[256];
    while (fgets(buf,sizeof(buf),stdin) != NULL)
    {
       buf[strcspn(buf,"\n")] =  '\0';   //读完后先去掉\n
       if(strcmp(buf,"quit") == 0){
        break; 
       }
       size_t total_in = 0;
       size_t len = strlen(buf);
       while (total_in < len)
       {
            ssize_t n_s = send(fd,buf+total_in,len - total_in,0);
            if(n_s < 0){
            perror("send");
            close(fd);
            return 1;
            }else if(n_s == 0 ){
                fprintf(stderr,"send unsuccessed");
                close(fd);
                return 1 ;
            }else{
                total_in +=(ssize_t)n_s;
            }
       
        }

         //接收回显
        char buf_back[256];
        ssize_t n_back;
        size_t total_out = 0;
        while(len > total_out){
            n_back = recv(fd,buf_back+total_out,sizeof(buf_back)-1-total_out,0);
            if( n_back < 0){
                perror("recv");
                close(fd);
                return 1;
            }
            else if( n_back == 0){
                printf("服务器端关闭连接\n");
                break;
            }else{
            total_out+=(size_t)n_back;
           
           
            }
            
        }
         buf_back[n_back] = '\0';
         printf("接收到回显%s\n",buf_back);
     
    }

    close(fd);
    return 0;
}