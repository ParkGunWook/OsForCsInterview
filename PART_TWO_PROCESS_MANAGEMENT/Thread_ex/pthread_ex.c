//gcc pthread.c -o pthread.exe -lpthread
//Must compile with option -lpthread
#include <pthread.h>
#include <stdio.h>

#include <stdlib.h>

int sum; //Shared with thread
void *runner(void *param);

int main(int argc, char *argv[])
{
    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, runner, argv[1]);

    pthread_join(tid, NULL);

    printf("sum =%d\n", sum);
}

void *runner(void *param)
{
    int i, upper = atoi(param);
    sum = 0;

    for(i = 1; i<=upper;i++){
        sum += i;
    }
    pthread_exit(0);
}