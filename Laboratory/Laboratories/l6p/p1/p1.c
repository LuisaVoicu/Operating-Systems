#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    pid_t p2=-1,p3=-1,p4=-1,p5=-1;
    int status=0;

    // doar p1
    printf("hello! i'm p1 with pid=%d\n",getpid());

    p2=fork();
    if(p2==0){

        p4=fork();
        if(p4==0){
            printf("hello! i'm process4 with id %d, my parent is %d\n",getpid(),getppid());
           sleep(60);

        }
        if(p4 > 0){
        waitpid(p4,&status,0);
        printf("hello! i'm process2 with id %d, my parent is %d\n",getpid(),getppid());
        }
    }
    else if(p2>0){

        // e necesar sa pun conditia , altfel si procesul p2 va crea alt proces p3 (codul e executat si de copil si de parinte)
        // sectiune de cod executata doar de parinte (p1)

    p3=fork();
    if(p3==0){
        p5=fork();
        if(p5==0){
            printf("hello! i'm process5 with id %d, my parent is %d\n", getpid(),getppid());
            sleep(60);
        }
        if(p5>0){
        waitpid(p5,&status,0);
        printf("hello! i'm process3 with id %d, my parent is %d\n",getpid(),getppid());
        }
        
    }
    else if(p3 > 0){
        // sectiune executata doar de parinte (p1)
        waitpid(p2,&status,0);
        printf("--> hello! i'm process1 with id %d, my first children is %d\n",getpid(),p2);

        wait(&status);
        printf("--> hello! i'm process1 with id %d, my second children is %d\n",getpid(),p3);
    }
    }

}
