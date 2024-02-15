#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    int n=7;
    int count_childs=0;
    int status=0;
    int i;
    pid_t pid=-1;
    pid_t parent_init=getpid();
 

   for(i=0;i<n;i++){
    //pt i=0 -- >  se creeaza un singur proces copil ; stim deci ca la i=0 getpid()=procesul initial

        pid = fork();
        if(pid==0){
            count_childs++;
            printf("%d+++++ [parent] %d ; [child] %d ; nb: %d %d\n",i,getppid(), getpid(), WEXITSTATUS(status), count_childs);
            if(i==n-1){
                //la n-1 nu se mai pot crea noi procese deci am numarat toate procesele noi create pe ramura curenta
                count_childs = count_childs%255;
                exit(count_childs); 
            }
        }
        else if(pid>0){
            // nu are sens sa numar aici procesele noi create (care stiu ca s-au incheiat) pentru ca voi ajunge sa numar de mai multe ori un proces
            waitpid(pid,&status,0);
            //primesc statusul (nr total de procese create pana in acel moment) unui proces care nu mai poate sa creeze alte procese (nv n-1)
            //si initializ count_child (in parinte!!!) cu acel numar astfel incat intre frati sa nu se piarda valoarea numarata pe alta ramura
       
        
            count_childs=WEXITSTATUS(status);
            printf("%d----- [parent] %d ; [child] %d ; nb: %d %d\n",i,getpid(), pid, WEXITSTATUS(status), count_childs);

        }
    }

    // pentru i = n-1 ajung aici
    // astept ca un proces din care nu se mai poate crea un proces copil (acesta pastreaza numarul total in status)
    wait(&status);
    printf(" [parent] %d ; [child] %d ; nb: %d %d %d\n",getpid(), pid, WEXITSTATUS(status), count_childs, parent_init);
    if(parent_init==getpid()){
        printf(" Numarul total de procese este: %d\n",WEXITSTATUS(status));
    }
    exit(WEXITSTATUS(status)); // trimit mai departe numarul curent numarat a.i. sa nu pierd valoarea intre ramuri (copiii copiaza datele din parinte - 
                                            //deci daca nu transmit parintelui mai departe valoare count_child e initializat cu 0 si pierde numarul de procese de pe ramura anterior vizitata)
    return 0;
}