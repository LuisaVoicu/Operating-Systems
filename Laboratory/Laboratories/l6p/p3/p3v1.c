#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>


int powof2(int x){
    int r = 1;
    for(int i=0;i<x;i++){
        r*=2;
    }
    return r;
}

int main(){
    int n=10;
    int count_childs=0;
    int status=0;
    int i;
    int pow=-1;
    int odd=0;
    pid_t pid=-1;

   for(i=0;i<n;i++){
        pid = fork();
if(pid==-1){
            printf("nope\n");
            exit(-1);
        }
        if(pid==0){
            //proces copil...
            //daca fac exit nu mai pot crea alte procese din copil..deci programul va putea crea procese doar din procesul initial
            //printf("---- [parent] %d ; [child] %d ; nb: %d\n",getppid(), getpid(), count_childs);
            int pow2 = powof2(i);
            exit(count_childs+pow2);
        }
        else if(pid>0){

            pow++;
            printf("=====> %d\n" ,pow);
            
            waitpid(pid,&status,0);
            // proces executat de parinte
            count_childs=WEXITSTATUS(status);
            printf("%d: +++++ [parent] %d ; [child] %d ; nb: %d -- %d ------>>>> %d\n",i,getpid(), pid, WEXITSTATUS(status),count_childs,pow);

        }


    }
    wait(&status);
    printf("[parent] %d ; [child] %d ; nb: %d\n",getpid(), pid, WEXITSTATUS(status));
   



    return 0;
}