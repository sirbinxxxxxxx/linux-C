#include "send_all.h"
#include <stdio.h>
int send_all(int client_fd,char * buf,ssize_t size)
{

            
            //发送回显
            size_t total = 0;
            while((size_t)size >total){
            ssize_t n_w=send(client_fd,buf+total,(size_t)size-total , 0);
            if(n_w < 0){
                perror("send");
                // close(client_fd);
                return -1;
            }else if(n_w == 0 ){
                fprintf(stderr,"写入失败\n");
                // close(client_fd);
                return -1;
            }else{
                total+=n_w;
            }
        }
        
            

        
        // close(client_fd);
        return 0;

}