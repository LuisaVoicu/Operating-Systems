#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>



int dir_count(const char *path){
    DIR * dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf; //acces metadate
    int cnt=0;
    dir=opendir(path);
    if(dir==NULL){
        perror("nu se poate deschide directorul");
        return;
    }

    while((entry = readdir(dir)) != NULL){
        char current_path[1000];
            snprintf(current_path,1000,"%s/%s",path,entry->d_name);
            if(!lstat(current_path,&statbuf) ){
                if(S_ISREG(statbuf.st_mode)){
                    cnt++;
                }
            }
    }
    closedir(dir);

        return cnt;

}
int main(){
    int n=10;
    int count_childs=0;
    int status=0;
    int i;
    pid_t pid=-1;
    pid_t parent_init=getpid();
    int fd;


   for(i=0;i<n;i++)
   {
        pid = fork();
        if(pid==0){
            //printf("%d+++++ [parent] %d ; [child] %d ; nb: %d %d\n",i,getppid(), getpid(), WEXITSTATUS(status), count_childs);
            char path[100];
            snprintf(path,1000,"/home/luisa/Desktop/l6p/p3/fis/%d.txt",getpid());
            int fd = creat(path,0600);
            if(fd==-1){
                printf(" nu\n");
            }


        }
        else if(pid>0){
            waitpid(pid,&status,0);

        }
    }

    wait(&status);
    if(getpid() == parent_init){
        //printf("parinte!");
        int cnt = dir_count("/home/luisa/Desktop/l6p/p3/fis");
        printf(" %d\n",cnt);
    }

    return 0;
}