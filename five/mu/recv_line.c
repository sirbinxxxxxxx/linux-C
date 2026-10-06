#include "recv_line.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
ssize_t recv_line(int client_fd,char *buf,size_t size)
{
    if (size == 0){
        return -1;
    }
      
    size_t total = 0;
    while (total < size - 1)
    {

        ssize_t n = recv(client_fd,buf+total,1,0);
        if( n < 0){
            if (errno == EINTR)
            continue;
            perror("recv");
            return -1;
        }
        else if( n == 0){
            break; ;
            }
        else{
             total+=(size_t)n;
            if(buf[total-1] == '\n'){
                
                break;
            }
           
            
        }
    }
    buf[total] = '\0';
    //printf("%s\n total = %d",buf,(int)total);
    return (ssize_t)total;
}