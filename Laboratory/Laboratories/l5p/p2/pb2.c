#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX_PATH_SIZE 10000
DIR* opendir(const char *name);
int closedir(DIR *dirp);
struct dirent *readdir(DIR *dirp);



int search_string(const char *path, const char* string){
    int fd=-1;
    fd = open(path,O_RDONLY);
    if(fd == -1){
        return 1;
    }
    int size_string = strlen(string);
    int size_file = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);
    int pos = 0;
    char* aux = (char*)malloc(sizeof(char)*(size_string+1));
    while(pos+size_string <= size_file){ 
        // pastrez in aux fiecare subsir de strlen(string) elemente ce incep de la pozitia pos=0,strnel(string)-1
        if(read(fd,aux,size_string)!=size_string){ 
            return 1;
        }
        aux[size_string]=0;
        if(!strcmp(aux,string)){
            return 0;
        }
        lseek(fd,pos,SEEK_SET); // mut inapoi cursorul la pozitia pos dupa ce a fost incrementat in urma citirii
        pos++;
    }
    return 1;
}

void dir_rec(const char *path, const char *file_name, const char *string, int *index){
    DIR * dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf; //acces metadate
    
    dir=opendir(path);
    if(dir==NULL){
        perror("nu se poate deschide directorul");
        return;
    }

    // citesc fiecare element 
    while((entry = readdir(dir)) != NULL){
        char current_path[MAX_PATH_SIZE];//(char*)malloc(sizeof(char)*MAX_PATH_SIZE);

        //ma asigur ca nu ma reintorc in directorul curent sau directorul parinte
        if(strcmp(entry->d_name,".") && strcmp(entry->d_name,"..")){

            //creez path-ul complet 
            snprintf(current_path,MAX_PATH_SIZE,"%s/%s",path,entry->d_name);

            //salvez in statbuf info despre fisier
            if(!lstat(current_path,&statbuf) ){
                if(S_ISREG(statbuf.st_mode)){

                    // verific daca fisierul la care am ajuns are acelasi nume ca cel dat in file_name
                    // si daca exista un string in acesta 
                    if(!strcmp(file_name,entry->d_name)&&!search_string(current_path,string)){
                        (*index)++;
                        char* aux = (char*)malloc(sizeof(char)*MAX_PATH_SIZE);
                        memset(aux,0,0);
                        // creez path-ul corespunzator symlink-ului
                        snprintf(aux,MAX_PATH_SIZE*2,"%s/%s.%d",path,file_name,*index);
                        symlink(current_path,aux);
                    }

                }else if(S_ISDIR(statbuf.st_mode)){
                    dir_rec(current_path,file_name,string,index);
                }
            }
        }
    }
    closedir(dir);
}


int main(int argc, char** argv){
    if(argc!=4){
        perror("numar incorect de argumente!");
        return 1;
    }
    int index=0;
   dir_rec(argv[1],argv[2],argv[3], &index);
   // ./pb2 /home/luisa/Desktop/l5p/p2/maindir file.txt hello

}