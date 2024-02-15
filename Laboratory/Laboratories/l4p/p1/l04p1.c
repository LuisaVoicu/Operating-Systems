#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>

int get_line(int fd, int lineNr, char *line, int maxLength){
    
    off_t size = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);

    int count_line = 0;
    int eof=false;
    off_t start_line = 0;
    while(count_line < lineNr){
        start_line = lseek(fd,0,SEEK_CUR);
        for(;;){
            char c;
  
            if(read(fd,&c,1)!=1){
                perror("nu se poate realiza citirea");
                return -1;
            }
            size--;
            if(size==0){ // end of file
                eof = true;
            }
            if(c=='\n'){
                count_line++;
                break;
            }

        }

        if(count_line==lineNr){
            off_t size_line = lseek(fd,0,SEEK_CUR)-start_line;
            if(size_line > maxLength){
                perror("dimensiune insuficienta!");
                return -1;
            }
            lseek(fd,start_line,SEEK_SET);
            if(read(fd,line,size_line)!= size_line){
                perror("nu se poate efectua citirea!");
                return -1;
            }
            return 0;

        }
        if(eof==true){
            perror("linii insuficiente");
            return -1;
        }
    }
}
int main(int argc, char** argv){
   /* if(argc!=3){
        perror("numar de argumente necorespunzator");
        return -1;
    }
    off_t nr_lin;
    sscanf(argv[2],"%ld",&nr_lin);
    int fd = open(argv[1],O_RDONLY);
    if(fd==-1){
        perror("nu se poate deschide fisierul de citire");
        return -1;
    }
    off_t max_length = 100;
    char* s = (char*)malloc(sizeof(char)*max_length);
    int result = get_line(fd,nr_lin,s,max_length);
    if(!result){
    printf("sir::: %s" ,s);
    }
    free(s);
    close(fd);*/

    
}