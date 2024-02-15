#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int  is_number(char* s){
    int i=0;
    while(s[i]){
        if(s[i]<'0'||s[i]>'9'){
            return 0;
        }
        i++;
    }
    return 1;
}

int main(){
    
    int status=0;
    char nb1[10], nb2[10], op[2];
    int done=0;
    memset(nb1,0,0);
    memset(nb2,0,0);
    memset(op,0,0);
   

    printf("press q to quit!\n");
    while(1){
        pid_t server_pid=0;
        printf("introduceri primul numar:\n");
        do{
            printf("> ");
            scanf("%s",nb1);
            if(!strcmp(nb1,"q")){
                done=1;
                break;
            }
        }while(!is_number(nb1));

        if(done){
            break;
        }

        printf("introduceri al doilea numar:\n");
        do{
            printf("> ");
            scanf("%s",nb2);
            if(!strcmp(nb2,"q")){
                done=1;
                break;
            }
        }while(!is_number(nb2));

        if(done){
            break;
        }

        printf("introduceti operandul + sau - :\n");
        do{
            printf("> ");
            scanf("%s",op);
            if(!strcmp(op,"q")){
                done=1;
                break;
            }
        }while(strcmp(op,"-")&& strcmp(op,"+"));


        if(done){
            break;
        }

    // scanf("%s %s %s",nb1,nb2,op); 
        printf("\n%s %s\n%s\n-----\n",nb1,op,nb2);
        server_pid = fork();

        if(server_pid==0){
            execl("/home/luisa/Desktop/l6p/p2/server","server",nb1,nb2,op,NULL);
            perror("nu pot deschide execl");
        }
        else{
                wait(&status);
                printf("%d\n",WEXITSTATUS(status));
        }
    }
}
