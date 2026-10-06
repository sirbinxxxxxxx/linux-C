#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

volatile sig_atomic_t tips = 0;

void handler(int sig) {
    tips =1 ;
}

int main (){

    int seconds =1;
    int exit_unm =1;

    struct sigaction sa ;
    memset(&sa,0,sizeof(sa));
    sa.sa_handler = handler;
    sa.sa_flags = 0 ;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGCHLD,&sa,NULL) == -1){
        perror("sigacation");
    }

    pid_t pid[5];
    for(int i = 0 ; i < 5 ; i++){
        pid[i] = fork();
        if(pid[i]==-1){
            perror("fork");
        }else if(pid[i]==0){
            printf("cheild %d running...\n",i);
            fflush(stdout);
            sleep(seconds+i);
            _exit((exit_unm+i)*10); 
        }else{}
        } 

        int finished = 0;
        while(1){
    
        
        printf("parent running ...\n");
                if(tips == 1){
                    int status ;
                    pid_t ret ;
                    while ( (ret = waitpid(-1,&status,WNOHANG))>0){                  
                    printf("回收PID:%d 退出码为：%d",(int)ret , WEXITSTATUS(status));
                    finished++;
                    }
                    tips = 0 ;
                    printf("%d",finished);
                }
   
       
    sleep(1);
    if(finished ==5){
        break;
     }
    }
    return 0 ;
}