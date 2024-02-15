#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int count;
    int count_childs=0;
int main(){
    int n=2;

    int status=0;
    int status2=0;
    int i;
    pid_t pid=-1;

   for(i=0;i<n;i++){
        pid = fork();
        pid_t pid2 = -1;
        if(pid==0){

            pid2=fork();

            if(pid2==0){
            exit(count_childs+1);
            }
            else if(pid2>0){
            waitpid(pid2,&status,0);
            count_childs=WEXITSTATUS(status)+1;
             printf("+++++ [parent] %d ; [child] %d ; nb: %d st: %d\n",getpid(), pid2, count_childs,  WEXITSTATUS(status));
            }
        }
        else if (pid > 0){
           waitpid(pid2,&status2,0);
           count_childs++; 
            printf("~~~~~~~>>> [parent] %d ; [child] %d ; nb: %d st: %d %d\n",getpid(), pid, count_childs,  WEXITSTATUS(status), WEXITSTATUS(status2));
            //exit(WEXITSTATUS(status2));

        }

    }

    waitpid(pid,&status,0);
    int status_result = WEXITSTATUS(status);
    printf("[parent: ] %d ; [child] %d ; nb: %d\n",getpid(), pid, status_result);

    return 0;
}