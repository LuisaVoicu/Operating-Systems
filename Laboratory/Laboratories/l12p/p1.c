#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd;
    off_t size;
    char *data = NULL;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
    {
        perror("Could not open input file");
        return 1;
    }
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    data = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == (void *)-1)
    {
        perror("Could not map file");
        close(fd);
        return 1;
    }

    int n = size - 1;
    for (int i = 0; i <= n / 2; i++)
    {
        char aux = data[i];
        data[i] = data[n - i ];
        data[n - i] = aux;
    }

    munmap(data, size);
    close(fd);


    return 0;
}
