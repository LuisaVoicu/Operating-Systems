#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("nr argumente invalid\n");
        return 0;
    }
    int fd1 = open(argv[1], O_RDWR);
    int fd2 = open(argv[2], O_RDWR);

    int size = lseek(fd1, 0, SEEK_END);
    lseek(fd1, 0, SEEK_SET);

    // mapez cate 500 octeti pana ce ajung la final

    int sz = 0;
    int x = 3;
    int index = 0;
     do
    {
        if (sz + x > size)
        {
            sz = size;
        }
        else
        {
            sz += x;
        }

        char *data1 = NULL;
        char *data2 = NULL;

        data1 = (char *)mmap(NULL, x, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
        data2 = (char *)mmap(NULL, x, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
      
 
        if (data1 == (void *)-1)
        {
            printf("fisierul 1 nu poate fi mapat");
            return 1;
        }

        if (data2 == (void *)-1)
        {
            printf("fisierul 2 nu poate fi mapat ca private");
            return 1;
        }

        printf("before\nd1: %s\nd2: %s\n\n\n", data1, data2);

        for (index = index; index < sz; index++)
        {
            char aux=0;
            aux = data1[index];
            data1[index]=data2[index];
            data2[index] =aux;
        }
    
        if (data1 == (void *)-1)
        {
            printf("fisierul 1 nu poate fi mapat ca shareed");
            return 1;
        }
        if (data2 == (void *)-1)
        {
            printf("fisierul 2 nu poate fi mapat ca shared");
            return 1;
        }

  

        printf("after\nd1: %s\nd2: %s\n\n", data1, data2);
        munmap(data1, x);
        munmap(data2, x);

    }  while (sz < size);
}