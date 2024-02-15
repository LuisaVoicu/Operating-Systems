#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{

    int fd_shm = -1;

    fd_shm = shm_open("/p2_shm", O_RDWR, 0600);

    if (fd_shm < 0)
    {
        perror("error:");
        return 1;
    }

    ftruncate(fd_shm, sizeof(int));

    volatile int *shared_int = NULL;
    shared_int = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if (shared_int == (void *)-1)
    {
        perror("error:");
        return 1;
    }

    *shared_int = 1;
    for (;;)
    {
        int previous = *shared_int;

        printf("[P2] x = %d\n", *shared_int);
        while (previous == *shared_int)
        {
            printf("[P2] waiting for p2_1 to increment...\n");
            sleep(1);
        }
        (*shared_int)++;
    }
}