#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#define N 16

int nrThreads;
sem_t sem;

void *limited_area(void *unused)
 {
sem_wait(&sem);
 nrThreads++;
 usleep(100);
 printf("The number of threads in the limited area is: %d\n", nrThreads);
 nrThreads--;
 sem_post(&sem);
 return NULL;
 }

int main(){

    pthread_t tid[N+1];
    int n;
    printf("nr max. threaduri: ");
    scanf("%d",&n);
    sem_init(&sem,0,n);

    for(int i = 0 ; i < N;i++){
        pthread_create(&tid[i],NULL,limited_area,NULL);
    }

    for(int i = 0 ; i< N;i++){
        pthread_join(tid[i],NULL);
    }


}
