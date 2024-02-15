#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>




int nr_lin(int fd){

int cnt_lin = 0;
bool eof=false;

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
    printf("abc %d\n",cnt_lin);
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
            return pos;
            break;
        }
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

}

int inv_lin(int fd, int fdo){
    int cnt = nr_lin(fd);
    for(int i = cnt-1; i>0;i--){
        int crt_lin = 0;

    }
}

int main(int argc, char** argv){
    /*if(argc!=3){
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
    printf("hello world?");
    //nr_lin(fd);
    off_t a = get_lin_n(fd,2);
    char c;
    while(write(fdo,&c,1)==1){
        printf("%c",c);
        //fflush();
    }
    printf("hello??");
    close(fd);
    close(fdo);*/
    


}
