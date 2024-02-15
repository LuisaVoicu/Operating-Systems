#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define DIM 100000

typedef struct {
    pthread_t tid;
    int cnt;
}Str_Thr;

void *threadMessage(void *param){
    for(;;){
        sleep(5);
    }
    return NULL;
}

int main()
{
    pthread_t* tid = (pthread_t*)malloc(sizeof(pthread_t*)*DIM);
    if(tid==NULL){
        perror("alocare memorie tid:");
        exit(1);
    }
    int n=0;

    while(!pthread_create(&tid[n++],NULL,threadMessage, &n)){
        if(n+2>DIM){
            //realocare
            tid = (pthread_t*)realloc(tid,sizeof(pthread_t*)*DIM);
            if(tid==NULL){
                perror("realocare memorie tid:");
                exit(2);
            }
        }
    }


    printf("Total number of threads created: %d\n",n);
    free(tid);
}