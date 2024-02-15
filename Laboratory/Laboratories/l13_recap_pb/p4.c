#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("nr incorect de argumente.\n");
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
    {
        printf("fisierul nu poate fi deschis.\n");
        return 1;
    }

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char *data = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == (void *)-1)
    {
        printf("fisierul nu poate fi mapat in memorie.\n");
        return 1;
    }

    char *result = NULL;
    char *aux = (char *)malloc((size + 1) * sizeof(char));

    do
    {
        result = strstr(data, argv[2]);

        if (result == NULL)
        {
            break;
        }

        int index = result - data;
        strcpy(aux, argv[3]);
        strcat(aux, data + index + strlen(argv[2]));
        strcpy(data + index, aux);
    } while (1);

    printf("\n\n%s\n", data);
    munmap(data, size);
    close(fd);
}