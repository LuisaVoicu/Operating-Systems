#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(){

    int status=0;
    pid_t pid1=-1;
    pid_t pid2=-1;
    pid_t pid3=-1;
    int fd;
    char path_to_aux[100];
    char path_to_second_aux[100];
    memset(path_to_aux,0,0);
    strcpy(path_to_aux,"/home/luisa/Desktop/l6p/p4/aux_output.txt\0");

    memset(path_to_second_aux,0,0);
    strcpy(path_to_second_aux,"/home/luisa/Desktop/l6p/p4/aux_output2.txt\0");

    //proces1: folosit pentru a afisa in fisier auxiliar toate liniile ce contin abcd -- grep
    pid1=fork();
    if(pid1==-1){
        perror("couldn't create first child\n");
        return -1;
    }
    else if(pid1==0){
        fd = open(path_to_aux, O_CREAT | O_TRUNC | O_WRONLY, 0644);    
        if(fd<0){
            perror("fisierul auxiliar nu se poate deschide!");
            exit(1);
        }
        
        dup2(fd,1); // redirectez output-ul iin fisier DOAR in procesul p1
        execlp("grep","grep","abcd","/home/luisa/Desktop/l6p/p4/file.txt",NULL);
        perror("can't execute grep");
        exit(2);
    }


    //proces2: folosit pentru a numara liniile din fisierul auxiliar
    waitpid(pid1,&status,0); // astept ca procesul 1 sa finalizeze
    pid2=fork();
    if(pid2==-1){
        perror("couldn't create child process");
        exit(3);
    }
    else if(pid2==0){
        fd=open(path_to_second_aux, O_CREAT|O_TRUNC|O_WRONLY,0644);
        if(fd<0){
            perror("fisierul aux2 nu se poate deschide");
            exit(1);
        }
        dup2(fd,1);
        execlp("wc","wc","-l",path_to_aux,NULL);
        perror("can't execute wc");
        exit(3);
    }

    // imi afiseaza nb_lines + file --> pot sa fac sa imi afiseze doar nb_lines cu cut -c 1 ==> proces 3 :)
                                            // deci si aici trebuie sa pun rezultatul intr-un fisier auxiliar

    waitpid(pid2,&status,0);
    pid3=fork();
    if(pid3==-1){
        perror("couldn't create child process");
        exit(3);
    }
    else if(pid3==0){
        execlp("cut","cut","-c","1",path_to_second_aux, NULL);
        perror("couldn't execute cut");
        exit(4);
    }

    waitpid(pid3,&status,0);
    printf("Status result: %d\n",WEXITSTATUS(status));
}