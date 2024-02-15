#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#define LOWER 5
#define UPPER 10000

typedef struct {
    int* arr;
    int from;
    int to;
}NB_DIG;

int totalNbOfDigits(int n){
    //todo fa-l sa numere bine dupa 100
    if(n<10){
        return n;
    }
    int count = 9;
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
    return (void*)(long)count;
}


int main(){

    FILE* fd = fopen("running_time.txt","w");
    if(fd==NULL){
        printf("eroare!\n");
        exit(1);
    }

    srand(time(NULL));

    int n = rand()%(UPPER-LOWER+1) + LOWER;;
    printf("n=%d\n",n);
    int nbTests = 10;
    for(int i = 0 ; i < nbTests; i++){

        clock_t start,end;
   

        int nbD = totalNbOfDigits(n);
        int* arr = (int*)calloc(nbD+2,sizeof(int));
        int nbThreads;

        do{
            nbThreads=rand()%(n-LOWER+1) + LOWER;
        }while(nbThreads>n);
        
        int nbElements = nbD/nbThreads;
        NB_DIG params[nbThreads];
        pthread_t tid[nbThreads];
        int index = 1;

        printf("n=%d\nnb Treads=%d\n",n,nbThreads);
       
        createArrayWithNbDigits(arr,nbD);


        start = clock();
        for(int i = 0; i < nbThreads; i++){
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
            if(i < nbD % nbThreads && j+1<=nbD){
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
            pthread_create(&tid[i],NULL,threadCountOnes,&params[i]);

        }

        void* nbResult;
        int countOnes=0;
        for(int i = 0 ; i < nbThreads; i++){
            pthread_join(tid[i],&nbResult);
            countOnes+=(int)(long)nbResult;
        }
        end = clock();


        printf("final answer: %d\n",countOnes);
        fprintf(fd,"final answer: %d\n",countOnes);
        free(arr);
        double duration = ((double)end - start)/CLOCKS_PER_SEC; 
        fprintf(fd,"Running time: %f\n", duration);
        fprintf(fd,"\n_______________________________________\n");

        printf("Running time: %f\n", duration);
        printf("\n_______________________________________\n");
    }

    long nb_nuclee = sysconf(_SC_NPROCESSORS_ONLN);
    printf("nr de nuclee: %ld\n",nb_nuclee);

}