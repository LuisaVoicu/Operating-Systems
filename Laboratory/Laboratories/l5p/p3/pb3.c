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

void dir_rec_del(const char *path){   
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf;
    char current_path[MAX_PATH_SIZE];

    dir = opendir(path);
    if(dir == NULL){
        printf("directorul nu poate fi deschis!");
        return;
    }
    while((entry = readdir(dir))!=NULL){
        
        if(strcmp(entry->d_name,".") && strcmp(entry->d_name,"..")){
            snprintf(current_path,MAX_PATH_SIZE,"%s/%s",path,entry->d_name);
            if(!lstat(current_path,&statbuf)){
                if(S_ISREG(statbuf.st_mode)){
                    unlink(current_path);
                }
                else if(S_ISDIR(statbuf.st_mode)){
                    dir_rec_del(current_path);
                }
            }
        }

    }
    closedir(dir);
    rmdir(path);
}




int main(int argc, char** argv){
    if(argc!=2){
        printf("numar incorect de argumente!");
        return 1;
    }
    dir_rec_del(argv[1]);
    // ./pb3 /home/luisa/Desktop/l5p/p3/dir1/dir2/dir3

}