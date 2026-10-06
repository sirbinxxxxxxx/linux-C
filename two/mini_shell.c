#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>   
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int last_result = 0;
    
    while (1)
    {   
        printf("mini$ ");
        fflush(stdout);

        char buf[256];    
            if(fgets(buf,sizeof(buf),stdin)==NULL){
                if(feof(stdin)){
                    break;
                }
                ferror(stdin);
                return 1;
            }

        int num = 0;
        char* args[100];

        char* token = strtok(buf,"\n \t");
        while (token!=NULL && num < 99 )
        {
            args[num] = token;
            num++;
            token =strtok(NULL,"\n \t");
        }
        args[num]=NULL;
        if(num == 0){
            continue;
        }
        if(strcmp(args[0], "exit") == 0){
            break;;
        }
        
        //内部命令用父进程
        if (strcmp(args[0], "cd") == 0) {
        if (num != 2) {
        fprintf(stderr, "用法：cd 目录\n");
        last_result = 1;
        continue;
        }

        if (chdir(args[1]) == -1) {
        perror("cd");
        last_result = 1;
        } else {
            last_result = 0;
        }

        continue;
       }

       //处理命令  >版本  + //处理命令 >>版本  +输入重定向 <
       int flge = 0;
       int index = -1;
       char *output_path = NULL;
       char *input_path = NULL;
       for(int i =0; i < num ;i++){
            if(strcmp(args[i],">")==0){
                index = i;
                 flge = 1;
                break;
            }
            if(strcmp(args[i],">>")==0){
                index = i;
                 flge = 2;
                break;
            }
            if(strcmp(args[i],"<")==0){
                index = i;
                flge = 3;
                break;
            }

       }
       if(index!=-1){
        if(index == 0||index+1>=num || index +2 !=num  ){
            if(flge == 1){
            fprintf(stderr,
            "用法：命令[参数...] > 文件\n"
            );
            }  
            else if(flge == 2){
                fprintf(stderr,
            "用法：命令[参数...] >> 文件\n"
            ); 
            }
            else if(flge == 3){
                 fprintf(stderr,
            "用法：命令[参数...] < 文件\n"
                 );
            }
           
        last_result =1 ;
        continue;
        }
        if(flge ==1 || flge ==2){
        output_path = args[index+1];
        }else if (flge == 3){
        input_path = args[index+1];
        }

        //截断命令
        args[index] = NULL;
       }


       
        //外部命令用fork
        pid_t pid = fork();
        if(pid < 0){
            perror("fork");
            return 1;
        }else if(pid == 0){
            if(output_path != NULL && index != -1){
                int fd ;
                if(flge == 1 || flge ==2){
                if(flge==1){
                     fd = open (output_path,O_RDWR|O_CREAT|O_TRUNC,0664);
                }else if(flge==2){
                     fd = open (output_path,O_RDWR|O_CREAT|O_APPEND,0664);
                }
                if(fd==-1){
                    perror("open");
                    _exit(2);
                    }
                if(dup2(fd,STDOUT_FILENO)==-1){
                perror("dup2");
                close(fd);
                _exit(3);
                }
                 close(fd);
                }
                }
                else if(input_path != NULL && index != -1){
                    int fd;
                    if(flge == 3){
                    fd = open(input_path,O_RDONLY);
                        if((fd==-1)){
                            perror("open");
                            _exit(4);
                        }
                     if(dup2(fd,STDIN_FILENO)==-1){
                        perror("dup2");
                        close(fd);
                        _exit(5);
                    }
                    close(fd);
                }
            
            }
            execvp(args[0],args);
            perror("execvp");
            _exit(127);
        }else{

            int status;
            pid_t ret_wait;
            while (1)
            {
                ret_wait = waitpid(pid,&status,0);
                if(ret_wait == -1){
                    if(errno==EINTR){
                        continue;
                    }
                    perror("waitpid");
                    return 1;
                }
                break;
            }
            if(WIFEXITED(status)){
                last_result = WEXITSTATUS(status);
                printf("命令执行完毕 返回码：%d\n",WEXITSTATUS(status));
            }  else if(WIFSIGNALED(status)){
                last_result=WTERMSIG(status)+128;
                printf("子进程被信号终止，信号编号为：%d\n",WTERMSIG(status));
                
            }

            
        }

    }
    

    return last_result;
}