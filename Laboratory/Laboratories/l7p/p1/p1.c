#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define NB_THREADS 10


typedef struct {
    int* arr;
    int from;
    int to;
}NB_DIG;

long totalNbOfDigits(int n){
    if(n<10){
        return n;
    }
    long count = 9;
    int  i=10;
    int x=2;
    int tenM = 10;
    while(i+tenM<=n){
        count+=x*tenM;
        i+=tenM;

        if(i%(tenM*10) == 0){
            tenM*=10;
            x++;
        }
    }
    if(i+tenM>n){
        count+=(n-i+1)*x;
    }
    return count;
}

void createArrayWithNbDigits(int* arr, int n){
    int cnt = 1;
    int x=1;
    int y=1;
    int tenM=10;
    for(int i = 1; i <= n ; i++){
        arr[i]=cnt;
        y--;
        if((cnt+1)%tenM==0){
            tenM*=10;
            x++;
        }
        if(y==0){
            cnt++;
            y=x;
        }
 
    }
}

void printArray(int* arr, int n){
    printf("\n");
    for(int i = 1; i<=n;i++){
        printf("%d:%d " ,i, arr[i]);
    }
    printf("\n");
}


void *threadCountOnes(void* param){
    NB_DIG* a = (NB_DIG*)param;
    int count = 0;
    for(int i = a->arr[a->from]; i<=a->arr[a->to]; i++){
        int nb = i;
        while(nb!=0){
            if(nb%10==1){
                count++;
            }
            nb/=10;
        }
    }
    return (void*)(long)count; //nr octeti diferiti pt 65
}

int main(){
    int n;
    scanf("%d",&n);
    long nbD = totalNbOfDigits(n);
    int* arr = (int*)calloc(nbD+2,sizeof(int));
    int nbElements = nbD/NB_THREADS;
    NB_DIG params[NB_THREADS];
    pthread_t tid[NB_THREADS];
    int index = 1;

    createArrayWithNbDigits(arr,nbD);
    //printArray(arr,nbD);
    printf("nb digits: %ld ", nbD);
    printf("\n\n");
    printf("nb elem in thread (aprox): %d\n" ,nbElements);
    printf("\nsections:\n");


    for(int i = 0; i < NB_THREADS; i++){
        int j;
        params[i].arr = arr;
        params[i].from = index;
        
        if(i==0){
            params[i].from = 1;
        }
        else{
            params[i].from = params[i-1].to+1;
        }
        
        j = params[i].from + nbElements-1;
        if(i < nbD % NB_THREADS && j+1<=nbD){
            j++;
        }
        // ma asigur ca voi lua numarul complet
        while(j+1<=nbD && arr[j]==arr[j+1]){
                    j++;
        }

        //doar pt ultimul grup
        if(j>nbD){
            j=nbD;
        }

        params[i].to = j;
        printf("%d %d --> %d \n" ,params[i].from, params[i].to, params[i].to-params[i].from);
        pthread_create(&tid[i],NULL,threadCountOnes,&params[i]);

    }

    void* nbResult;
    int countOnes=0;
    for(int i = 0 ; i < NB_THREADS; i++){
        pthread_join(tid[i],&nbResult);
        countOnes+=(int)(long)nbResult;
        //printf("%d: %d\n",i,countOnes);
    }
    printf("\nfinal answer: %d\n",countOnes);
    free(arr);
}