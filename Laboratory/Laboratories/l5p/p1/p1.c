#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX_PATH_SIZE 10000
#define MAX_OFFSET 10000000



void rec_dir(const char* path, off_t* size,int* freq){
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char current_path[MAX_PATH_SIZE];
    struct stat statbuf;
    
    dir=opendir(path);
    if(dir==NULL){
        printf("directorul nu poate fi deschis");
        return;
    }

    while((entry = readdir(dir))!=NULL){
        if(strcmp(entry->d_name,".")&&strcmp(entry->d_name,"..")){
            snprintf(current_path, MAX_PATH_SIZE,"%s/%s",path,entry->d_name);
            if(!lstat(current_path,&statbuf)){
                if(S_ISREG(statbuf.st_mode)){
                    if(!freq[statbuf.st_ino]){
                    (*size)+=statbuf.st_size;
                    freq[statbuf.st_ino]=1;
                    }
                    printf("%s %ld;;; inode id: %ld\n",current_path, statbuf.st_size, statbuf.st_ino);

                }
                else if(S_ISLNK(statbuf.st_mode)){
                    printf("link: %ld\n",statbuf.st_size);
                }
                else if(S_ISDIR(statbuf.st_mode)){
                    rec_dir(current_path,size,freq);
                }
            }
        }
    }
    closedir(dir);
}

off_t dirSize(const char* dir_path){
    off_t size = 0;
    int* freq = (int*)malloc(sizeof(int)*MAX_OFFSET);
    memset(freq,0,0);
    rec_dir(dir_path,&size,freq);
    free(freq);
    return size;
}


int main(int argc, char** argv){
    if(argc!=2){
        printf("numar de argumente invalid!");
    }

    // creez cateva symlink-uri 
    symlink("/home/luisa/Desktop/l5p/p1/dir1/dir2/file2.txt","/home/luisa/Desktop/l5p/p1/dir1/file2.txt");
    symlink("/home/luisa/Desktop/l5p/p1/dir1/dir3/file3.txt","/home/luisa/Desktop/l5p/p1/dir1/file3.txt");
    //hardlinks
    link("/home/luisa/Desktop/l5p/p1/dir1/dir2/file23.txt","/home/luisa/Desktop/l5p/p1/dir1/file23.txt");
    printf("size of %s is %ld\n",argv[1],dirSize(argv[1]));
}
