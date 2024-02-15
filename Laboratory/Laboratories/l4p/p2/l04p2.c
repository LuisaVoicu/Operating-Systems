#include <stdio.h>
#include <fcntl.h>
int main(int argc, char** argv)
{
    int fdi,fdo;

    if(argc!=3){
        fprintf(stderr,"numar nepotrivit de argumente pe linia de comanda");
        return 1;
    }
    printf("%s %s\n" ,argv[1],argv[2]);
    fdi = open(argv[1],O_RDONLY);
    if(fdi==-1){
        perror("nu se poate deschide fisierul de citire\n");
        return 1;
    }
    fdo = creat(argv[2],O_WRONLY);
    if(fdo==-1){
        perror("nu se poate crea fiisierul de scriere\n");
        return 1;
    }

    off_t size = lseek(fdi,0,SEEK_END);
    off_t pos = size;
    lseek(fdi,0,SEEK_SET);
    for(int i = 0 ; i <size; i++){
        char c;
        if(read(fdi,&c,1)!=1){
            perror("eroare la citire");
            break;
        }
        lseek(fdo,pos,SEEK_SET);
        if(write(fdo,&c,1)!=1){
            perror("eroare la scriere!");
            break;
        }
        pos--;
    }
return 0;

}
