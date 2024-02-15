#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int is_digit(char a)
{
    if(a>='0' && a<='9')
    {
        return 1;
    }
    return 0;
}

int main()
{
    int fd_pipe[2]; // 1 scrie, 0 citeste
    int pid;

    if (pipe(fd_pipe) != 0)
    {
        printf("pipe-ul nu poate fi creat!");
        return 1;
    }
    pid = fork();
    if (pid == -1)
    {
        printf("procesul copil nu poate fi creat");
        return 1;
    }

    if (pid == 0)
    {
        // proces copil
        close(fd_pipe[1]); // nu trebuie sa scriu in copil
        char x;
         do
        {
            read(fd_pipe[0], &x, sizeof(char));
           // printf("[C] %c %d\n",x, x);
            int nb = x-'0';
            if(x%2==0)
            {
                printf("PAR: %d\n",nb);
            }

        } while (x != '0'); 
    }
    else if(pid > 0)
    {
        //deschid fisierul binar

        close(fd_pipe[0]);

        int fd_file = open("file1.txt",O_RDONLY);
        if(fd_file<0)
        {
            printf("fisierul nu poate fi deschis");
            return 1;
        }
        
        int size = lseek(fd_file,0,SEEK_END);
        lseek(fd_file,0,SEEK_SET);
        for(int i = 0 ; i < size; i++)
        {
            char c = 0;
            read(fd_file,&c, sizeof(char));
            if(is_digit(c))
            {
                write(fd_pipe[1],&c,sizeof(char));
               // printf("[P] %c %d\n",c,c);
            }
        }

    }
}