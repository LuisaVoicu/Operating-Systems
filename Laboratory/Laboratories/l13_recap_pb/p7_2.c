#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_NAME "l13_fifo"

int main()
{
    int fd_fifo = -1;
    fd_fifo = open(FIFO_NAME, O_RDONLY);
    if (fd_fifo == -1)
    {
        printf("pipe-ul nu poate fi deschis in p1");
        return 1;
    }

    char s[100];
    while (read(fd_fifo, s, 100) > 0)
    {


        if (s[0] >= '0' && s[0] <='9')
        {
            printf("%s\n", s);
        }
    }
    close(fd_fifo);
}