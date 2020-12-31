#include <pthread.h>
#include <stdio.h>
#define NUM_THREAD 5

void *runner(void *param)
{
    printf("Runner go\n");
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    int i, policy;
    pthread_t tid[NUM_THREAD];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    if(pthread_attr_setschedpolicy(&attr, SCHED_FIFO) != 0)
        fprintf(stderr, "Unable to set policy\n");

    if(pthread_attr_getschedpolicy(&attr, &policy) != 0){
        printf("Unable to get scheduling policy\n");
    }
    else{
        if(policy == SCHED_OTHER)
            printf("SCHED_OTHER\n");
        else if(policy == SCHED_FIFO)
            printf("SCHED_FIFO\n");
        else if(policy == SCHED_RR)
            printf("SCHED_RR\n");
        else
        {
            printf("Illegal policy value\n");
        }
    }

    for(int i=0; i< NUM_THREAD;i++){
        pthread_create(&tid[i], &attr, runner, NULL);
    }
    for(int i=0; i<NUM_THREAD;i++){
        pthread_join(tid[i], NULL);
    }
}