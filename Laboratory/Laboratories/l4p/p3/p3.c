#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>




int nr_lin(int fd){

int cnt_lin = 0;
bool eof=false;

    lseek(fd,0,SEEK_SET);
    while(true){

        char c;
        do{
            if(read(fd,&c,1)!=1){
                eof = true;
                break;
            }
        }while(c!='\n');

        if(eof == true){
            break;
        }
        cnt_lin++;
    }
    lseek(fd,0,SEEK_SET);
    return cnt_lin;
}

off_t get_lin_n(int fd, int n){
    int cnt_lin = 0;
    off_t pos;
    bool eof=false;

    lseek(fd,0,SEEK_SET);
    while(true){
        if(cnt_lin+1 == n){
            pos = lseek(fd,0,SEEK_CUR);
            lseek(fd,0,SEEK_SET);
            return pos;
        }
        char c;
        do{
            if(read(fd,&c,1)!=1){
                eof = true;
               lseek(fd,0,SEEK_SET);

               return -1;
            }
        }while(c!='\n');

        if(eof == true){
            return -1;
        }
        cnt_lin++;
    }

}

int inv_lin(int fd, int fdo){
    int nr_total = nr_lin(fd);
    for(int i = nr_total; i>=1;i--){
        off_t pos = get_lin_n(fd,i);
        lseek(fd,pos,SEEK_SET);
        char c;
        while(read(fd,&c,1)==1){
            if(write(fdo,&c,1)!=1){
                perror("Eroare.Nu se poate scrie in fisier de iesire!");
                return -1;
            }
            if(c=='\n'){
                break;
            }
        }
    }
    return 0;
}

int main(int argc, char** argv){
    if(argc!=3){
        perror("numar de argumente necorespunzator");
        return -1;
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd==-1){
        perror("nu se poate deschide fisierul de citire");
        return -1;
    }
    int fdo = open(argv[2],O_RDWR);
    if(fdo==-1){
        perror("nu se poate deschide fisierul de scriere");
        return -1;
    }
    int ans = inv_lin(fd,fdo);
    close(fd);
    close(fdo);
    return ans;

}