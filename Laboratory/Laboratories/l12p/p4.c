#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#define MAX_VEC 10000

int prime(int n)
{
    if (n < 2)
        return 0;

    if (n == 2)
        return 1;

    for (int i = 2; i < n; i++)
    {
        if (n % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv)
{
    int a = 0;
    int b = 0;

    if (argc != 3)
    {
        printf("numar incorect de argumente in programul secundar");
        return 1;
    }
    sscanf(argv[1], "%d", &a);
    sscanf(argv[2], "%d", &b);
    printf("Eu sunt programul secundar cu argumentele %d %d si am gasit urmatoarele nr prime: ", a, b);

    sem_t *sem = NULL;
    sem = sem_open("/sem_l12", 0);
    if (sem == NULL)
    {
        printf("semaforul nu poate fi deschis din programul secundar");
        return 1;
    }

    int fd_shm = shm_open("/l12_p4", O_RDWR, 0600);
    if (fd_shm < 0)
    {
        printf("nu se poate deschide zona de memorie partajata .");
        return 1;
    }

    int *vec = NULL;
    vec = mmap(0, sizeof(int) * MAX_VEC, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    sem_wait(sem); // il pun aici a.i. procesul curent sa isi scrie toate rezultatele
    for (int i = a; i <= b; i++)
    {
        if (prime(i) == 1)
        {
            printf("%d ", i);
            vec[0]++;
            vec[vec[0]] = i;
        }
    }

    printf("\n");
}