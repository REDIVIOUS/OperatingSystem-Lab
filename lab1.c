#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

pid_t pid1,pid2;  //两个子进程
int cnt=1; //子进程1像子进程2发送信号的次数
char info_1[50],info_2[50]; //子进程1发送的数据和子进程2读到的数据

//捕获键盘中断信号后的动作
void signal_get(int sig_no){ 
    if(sig_no==SIGUSR1){ //捕捉SIGUSR1信号，结束子进程1
        printf("Child Process 1 is Killed by Parent!\n");
        exit(0);
    }
    if(sig_no==SIGUSR2){ //捕捉SIGUSR2信号，结束子进程2
        printf("Child Process 2 is Killed by Parent!\n");
        exit(0);
    }
    if(sig_no==SIGINT){ //捕捉SIGINT信号，对pid1和pid2发出kill信号
        kill(pid1,SIGUSR1);
        kill(pid2,SIGUSR2);
        return ;
    }
}

int main(){
    int pipefd[2]; //pipefd[0]用于读，pipe[1]用于写
    pipe(pipefd); //创建无名管道
    pid1=fork();
    if(pid1==0){
        signal(SIGINT,SIG_IGN); //忽略SIGINT信号
        signal(SIGUSR1,signal_get);
        for(;;){
            sprintf(info_1,"I send you x times.\n",cnt); //写入子进程1向子进程2通信次数
            write(pipefd[1],info_1,sizeof(info_1)); //写入写管道
            sleep(1); //每隔一秒发送一次
            cnt++;
        }
    }
    else{
        pid2=fork();
        if(pid2==0){
            signal(SIGINT,SIG_IGN); //忽略SIGINT
            signal(SIGUSR2,signal_get); //捕捉SIGUSR2
            for(;;){ 
                read(pipefd[0],info_2,sizeof(info_2)); //写入读管道
                sleep(1);
                printf("%s",info_2); //屏幕上输出通信内容
        }
        else{
            int pid1_code,pid2_code; //储存pid1和pid2的退出码
            waitpid(pid1,&pid1_code,0); //等待子进程1的结束
            waitpid(pid2,&pid2_code,0); //等待子进程2的结束
            close(pipefd[0]); //关闭管道读端口
            close(pipefd[1]); //关闭管道写端口
            printf("Parent Process is Killed!");
            return 0；
        }
        }
    }
}