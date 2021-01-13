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
    int i, scope;
    pthread_t tid[NUM_THREAD];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    if(pthread_attr_getscope(&attr, &scope) != 0){
        printf("Unable to get scheduling scope\n");
    }
    else{
        if(scope == PTHREAD_SCOPE_PROCESS)
            printf("PTHREAD_SCOPE_PROCESS\n");
        else if(scope == PTHREAD_SCOPE_SYSTEM)
            printf("PTHREAD_SCOPE_SYSTEM\n");
        else
        {
            printf("Illegal scope value\n");
        }
    }
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    for(int i=0; i< NUM_THREAD;i++){
        pthread_create(&tid[i], &attr, runner, NULL);
    }
    for(int i=0; i<NUM_THREAD;i++){
        pthread_join(tid[i], NULL);
    }
}