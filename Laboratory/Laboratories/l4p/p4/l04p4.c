
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
int main(int argc, char** argv){
    int fd;
    //argumentele primite sunt nume fisier , string, pozitie
    if(argc!=4){
        perror("numar de argumente invalid!");
        return -1;
    }
    
    fd = open(argv[1],O_RDWR);
    if(fd==-1){
        perror("nu se poate deschide fisierul!");
        return -1;
    }
    char* ch = argv[2];
    off_t pos=0;
    sscanf(argv[3],"%ld",&pos);

    off_t size = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);
    
    if(pos > size){
        perror("pozitia prea mare!");
        return -1;
    }
    int size_ch = strlen(ch);
    off_t new_size = size + size_ch;
    off_t index = size;
    for(int i = new_size-1; i>=new_size-(size-pos); i--){
        char c;
        index--;
        lseek(fd,index,SEEK_SET);
        if(read(fd,&c,1)!=1){
            perror("nu se poate citi din fisier!");
            return -1;
        }
        lseek(fd,i,SEEK_SET);
        if(write(fd,&c,1)!=1){
        perror("nu se poate scrie in fisier!");
        return -1;
        }
    }
    lseek(fd,pos,SEEK_SET);
    if(write(fd,ch,size_ch)!=size_ch){
        perror("nu se poate insera sirul in fisier!");
        return -1;
        }
    return 0;

}