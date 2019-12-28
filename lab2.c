#include <stdio.h>
#include <pthread.h>
#include<sys/types.h>
#include<linux/sem.h>

int semid; //信号灯集合
pthread_t p1,p2; //程序句柄
int a; //公共变量a,存放运算和

//P、V操作函数
void P(int semid,int index){
    struct sembuf sem;
    sem.sem_num=index; //该操作在信号量集中的索引为index（下标）
    sem.sem_op=-1; //要执行操作为减一
    sem.sem_flg=0; //操作标记：0或IPC_NOWAIT等
    semop(semid,&sem,1); //1是执行命令的个数
    return;
}
void V(int semid,int index){
    struct sembuf sem;
    sem.sem_num=index; //该操作在信号量集中的索引为index（下标）
    sem.sem_op=1; //要执行操作为加一
    sem.sem_flg=0; //操作标记：0或IPC_NOWAIT等
    semop(semid,&sem,1); //1是执行命令的个数
    return;
}

//线程执行函数
//subp1负责计算1到100的累加
void *subp1(void *arg){
    int i=0;
    for(i=1;i<=100;i++){
        P(semid,0); 
        a+=i;
        printf("thread 1: I am counting...\n");
        V(semid,1);
    }
    return NULL;
}
//subp2负责打印中间结果
void *subp2(void *arg){
    int i=0;
    for(i=1;i<=100;i++){
        P(semid,1);
        printf("thread 2: Cunrrent sum is: %d\n",a); //打印中间结果
        V(semid,0);
    }
    return NULL;
}

int main()
{
    //创建一个信号量集，创建两个信号灯(用于P和V)
    semid=semget(IPC_PRIVATE,2,IPC_CREAT | 0666); //两个信号灯分别为能从打印到计算和从计算到打印
    //信号量的赋值
    semctl(semid,0,SETVAL,1); //0号信号灯初始化为1
    semctl(semid,1,SETVAL,0); //1号信号灯初始化为0
    //创建线程
    pthread_create(&p1,NULL,subp1,NULL); //计算线程
    pthread_create(&p2,NULL,subp2,NULL); //打印线程
    //等待两个线程运行结束
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);
    //删除信号灯
    semctl(semid,0,IPC_RMID); //删除信号灯
    return 0;
}


