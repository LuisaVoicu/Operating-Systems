#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>


void *threadShow(void* param){
    int *nb = (int*) param;
    int iNb = *nb;
    for(;;){
        printf("I'm %d\n",iNb);
        int random = rand() % 5 + 1; 
        sleep(random);
    }
    return NULL;
}

int main(){

    pthread_t tid[10];
    int nbThread[10];
    int frequency[11];
    int nb;
    memset(frequency,0,10);

    for(int i = 0 ; i < 9; i++){
        nbThread[i]=i+1;
    }

    srand(time(NULL));
    for(int i = 0; i<9; i++){
        pthread_create(&tid[i],NULL,threadShow,&nbThread[i]);

    }

    for(;;){
        scanf("%d",&nb);
        printf("introdus- %d\n",nb);
        if(nb>=1&&nb<=9){
            pthread_cancel(tid[nb-1]);
            frequency[nb]=1;
        }
        int all = 1;
        for(int i = 1;i<10;i++){
            if(frequency[i]==0){
                all=0;
                break;
            }
        }
        if(all == 1){
            break;
        }
    }
    for(int i=0;i<9;i++){
       pthread_join(tid[i],NULL);
    }

    //exit main thread 
    pthread_exit(NULL); 
}
