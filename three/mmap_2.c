 #include <sys/mman.h>
#include <stdio.h>
  #include <sys/types.h>
       #include <unistd.h>
 #include <string.h>
       #include <sys/wait.h>

    struct  share_date 
    {
        int ready ;
        int value;
        char message[100]; 
    };
    

 int main()
 {
    struct  share_date date ;
   
    struct share_date* map = mmap(
                        NULL,
                        sizeof(struct share_date ),
                        PROT_WRITE | PROT_READ,
                        MAP_SHARED|MAP_ANONYMOUS,
                        -1,
                        0

                );
   
            if(map == MAP_FAILED){
                perror("mmap");
                return 1;
            }
            map->value =0;
            map->ready = 0;
            memset(&map->message,0,sizeof(map->message));
           
            

        __pid_t pid = fork();
        if(pid == -1){

            perror ("fork");
            return 1;
        }

        else if (pid == 0){
            printf("child: 开始写共享内存\n");
           
            map->value = 1;
            map->ready = 1;
   
            strcpy(map->message,"hello parent");
            printf("child: child: 写入完成\n");
            fflush(stdout);

             munmap(map,sizeof(struct  share_date));

            _exit(2);

        }else{
            while (map->ready ==  0)
            {
              printf("数据还没准备好\n");
              sleep (1);
            }
        }
        
      
            
        
           printf("ready:%d\n",map->ready);
          printf("value:%d\n",map->value);
           printf("message:%s\n",map->message);

        munmap(map,sizeof(struct  share_date));
    return  0 ;
 }

