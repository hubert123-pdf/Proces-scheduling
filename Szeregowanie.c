#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Proces
{
    int pid;
    int arrival_time;
    int burst_time;
    int completed_time;
    int priority;
    int if_completed;
    int if_waiting;
}*p,*ptr;

int k;
int time2=0,size=0,process_num=0,completed_tasks=0;
pthread_mutex_t mutex,mutex2;

void* Read(void* arg)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t lineSize = 0;
    int time1;
    while((lineSize=getline(&line, &len, stdin))!=-1){
        int new_processes_number;
        if (lineSize>3){
            int iterator=0,buff=size;
            new_processes_number=(lineSize-3)/5;    
            size+=new_processes_number;
            p=realloc(p,sizeof(struct Proces)*size);
            while(iterator!=new_processes_number){
                p[buff+iterator].arrival_time=time1;
                p[buff+iterator].pid=line[2+6*iterator]-'0';
                p[buff+iterator].priority=line[4+6*iterator]-'0';
                p[buff+iterator].burst_time=line[6+6*iterator]-'0';
                p[buff+iterator].completed_time=0;
                iterator++;
            }
        }
        time1++;
    }
}
void* FCFS(void* arg)
{
    pthread_mutex_lock(&mutex);
    if(k!=process_num)
        printf("%d ",-1);
    else{
        printf("%d ",process_num+1);
        completed_tasks++;
        if (p[process_num].burst_time==completed_tasks){
            k++;
            completed_tasks=0;
        }
    }
    pthread_mutex_unlock(&mutex);
}
void* RR(void *arg)
{
    int no_found1=0,no_found2=0;
    for (int i = 0; i < size; i++){
        if (p[i].arrival_time==time2){
            p[i].if_waiting=1;
        }
    }
    pthread_mutex_lock(&mutex); 
    ptr++;
    int i=0;
    while(ptr->if_waiting!=1 || ptr->if_completed!=0){ 
        i++;
        ptr++;
        if(i==size){
            ptr=&p[0];
            no_found1=1;
            break;
        }
        no_found1=0;
    }
    if(no_found1==1){
        i=0;
        while(ptr->if_waiting!=1 || ptr->if_completed!=0){
            ptr++;
            i++;
            if (i==size)
            {
                no_found2=1;
                break;
            }
            no_found2=0;
        }
    }
    pthread_mutex_unlock(&mutex);
    if(no_found2==1 && no_found1==1)
    {
        printf("%d ",-1);
    }
    else{
        printf("%d ",ptr->pid);
        pthread_mutex_lock(&mutex2);
        ptr->completed_time++; 
        if(ptr->completed_time==ptr->burst_time){
            ptr->if_completed=1;
            completed_tasks++;
        }
        pthread_mutex_unlock(&mutex2);
    }
}
void* FCFS_priority(void* arg)
{
    int no_found1=0,no_found2=0;
    for (int i = 0; i < size; i++){
        if (p[i].arrival_time==time2){
            p[i].if_waiting=1;
        }
    }
    pthread_mutex_lock(&mutex); 
    for (int i = 0; i < size; i++){
        if(p[i].if_waiting && !p[i].if_completed && (ptr->priority<p[i].priority || ptr->if_completed))
        {
            ptr=&p[i];
        }   
    }
    printf("%d ",ptr->pid);
    ptr->completed_time++; 
    if(ptr->completed_time==ptr->burst_time){
        ptr->if_completed=1;
        completed_tasks++;
    }
    pthread_mutex_unlock(&mutex);
}
int main(int argc, char*argv[])
{
    k=process_num;
    int strategy=atoi(argv[1]),num_threads=atoi(argv[2])+1;
    pthread_t thread[num_threads+1];
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);
    p=malloc(sizeof(struct Proces));

    pthread_create(&thread[0],NULL,Read,NULL);
    pthread_join(thread[0],NULL);

    switch (strategy)
    {
    case 0:
        while(process_num<size){
            printf("%d ",time2);
            for(int i=1;i<num_threads;i++)
            {
                pthread_create(&thread[i],NULL,FCFS,NULL);
            }
            for(int i=1;i<num_threads;i++)
            {
                pthread_join(thread[i],NULL);
            }
            printf("\n");
            if(k!=process_num){
                process_num++;
                k=process_num;
            }
            time2++;
        }
        break;  
    case 3:
        ptr=&p[0];
        while(completed_tasks!=size){
            printf("%d ",time2);
            for(int i=1;i<num_threads;i++)
            {
                pthread_create(&thread[i],NULL,RR,NULL);
            }
            for(int i=1;i<num_threads;i++)
            {
                pthread_join(thread[i],NULL);
            }
            printf("\n");
            time2++;
        }
        break;
    case 4:
    {
        ptr=&p[0];
        while(completed_tasks!=size){
            printf("%d ",time2);
            for(int i=1;i<num_threads;i++)
            {
                pthread_create(&thread[i],NULL,FCFS_priority,NULL);
            }
            for(int i=1;i<num_threads;i++)
            {
                pthread_join(thread[i],NULL);
            }
            printf("\n");
            time2++;
        }
        break;
    }
    default:
        break;
    }
    free(p);
}