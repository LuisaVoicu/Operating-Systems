#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
   
    if(argc!=4){
        perror("nr argumente incorect in server!\n");
        exit(1);
    }

    int nb1=0,nb2=0;
    sscanf(argv[1],"%d",&nb1);
    sscanf(argv[2],"%d",&nb2);
    
    if(!strcmp(argv[3],"+")){
        // exit returneaza unsigned char-uri
        exit(nb1+nb2); 

    }
    else if(!strcmp(argv[3],"-")){
        exit(nb1-nb2);
    }
   // exit(-1);
}