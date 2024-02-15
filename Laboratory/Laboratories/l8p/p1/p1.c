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

int N=4, M=100;
long count;
sem_t sem;

void *thread_function(void *unused)
{
    int i;
    long aux;

    sem_wait(&sem); //permit unui singur thread sa accezese zona critica
    for (i = 0; i < M; i++)
    {
        aux = count;
        aux++;
        usleep(random() % 10);
        count = aux;
    }
    sem_post(&sem);
    return NULL;
}

int main()
{
    // ce valoare ar trebui sa aiba count la final? R:M*N
    // fara sincronizare are de fapt valoarea ~M pentru ca thread-urile sunt executate in paralel si count ramane 0
    pthread_t tid[N+1];
    sem_init(&sem,0,1); 

    for(int i=0;i<N;i++){
        pthread_create(&tid[i],NULL,thread_function,NULL);
    }

    for(int i=0;i<N;i++){
        pthread_join(tid[i],NULL);
    }
    printf("%ld\n",count);
}