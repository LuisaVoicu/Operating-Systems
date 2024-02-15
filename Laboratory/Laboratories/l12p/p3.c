#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Numar insuficient de argumente!\n");
        return 1;
    }

    int fd_map = open(argv[1], O_RDWR);
    if (fd_map == -1)
    {
        printf("Fisierul nu poate fi deschis!\n");
        return 1;
    }

    off_t size = lseek(fd_map, 0, SEEK_END);
    lseek(fd_map, 0, SEEK_END);

    char *data = NULL;
    data = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_map, 0);
    if (data == (void *)-1)
    {
        printf("Fisierul nu poate fi mapat!\n");
        return 1;
    }

    printf("before:\n%s.\n",data);

    char *aux = (char *)malloc(sizeof(char) * size);

    for (int i = 0; i < size; i++)
    {
        if (strchr("aeiouAEIOU", data[i]) != 0)
        {
            strcpy(aux, data + i + 1);
            strcpy(data+i,aux);
            i--;
            data[size-1]=' ';
        }
    }

    printf("after:\n%s.\n",data);

    munmap(data,size);
    close(fd_map);

}