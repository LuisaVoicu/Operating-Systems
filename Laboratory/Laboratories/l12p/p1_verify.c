#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd;
    off_t size, i;
    char *data = NULL;



    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Could not open input file");
        return 1;
    }
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    data = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (data == (void *)-1)
    {
        perror("Could not map file");
        close(fd);
        return 1;
    }

  for(i=0;i<size;i++) {
        printf("%c", data[i]);
    }

    munmap(data, size);
    close(fd);

    return 0;
}