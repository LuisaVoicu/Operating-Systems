#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_VEC 10000
#define N 6

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("Numar de argumente incorect!\n");
        return 1;
    }

    sem_t *sem = NULL;
    sem = sem_open("/sem_l12", O_CREAT, 0644, 0);
    if (sem == NULL)
    {
        printf("semaforul nu poate fi creat din programul principal");
        return 1;
    }

    int a = 0;
    int b = 0;
    sscanf(argv[1], "%d", &a);
    sscanf(argv[2], "%d", &b);

    int fd_shm;

    fd_shm = shm_open("/l12_p4", O_CREAT | O_RDWR, 0600);
    if (fd_shm < 0)
    {
        printf("Nu se poate crea memorie partajata!\n");
        return 1;
    }

    ftruncate(fd_shm, sizeof(int) * MAX_VEC);

    int *shared_vect = NULL;
    shared_vect = (int *)mmap(0, sizeof(int) * MAX_VEC, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    if (shared_vect == (void *)-1)
    {
        printf("Nu se poate realiza maparea la memorie partajata!");
        return 1;
    }

    memset(shared_vect, 0, MAX_VEC*sizeof(int));

    for (int i = 1; i <= shared_vect[0]; i++)
    {
        printf("aaaa  %d ", shared_vect[i]);
    }

    int x = (b+a)/N;
    for (int i = a; i <= b; i += x)
    {
        int next = i + x - 1;
        if (next > b)
        {
            next = b;
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            printf("nu poate fi creat procesul\n");
            return 1;
        }
        else if (pid == 0)
        {
            char a_str[100], b_str[100];
            memset(a_str, 0, 100);
            memset(b_str, 0, 100);
            sprintf(a_str, "%d", i);
            sprintf(b_str, "%d", next);
            sem_post(sem);
            execlp("./p4", "p4", a_str, b_str, NULL);
            printf("Nu pot deschide programul secundar.");
        }
        else
        {
            waitpid(pid, NULL, 0);
            printf("done\n");
        }
    }

    printf("Numerele prime din intervalul %d %d sunt:\n", a, b);
    for (int i = 1; i <= shared_vect[0]; i++)
    {
        printf("%d ", shared_vect[i]);
    }
    printf("\n");
}