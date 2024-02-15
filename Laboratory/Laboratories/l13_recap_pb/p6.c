#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct THR
{
    int id;
    int start, end;
} THR;

sem_t sem;
int *result;
int cnt;

int perfect(int n)
{
    int sum = 0;

    for (int i = 1; i < n; i++)
    {
        if (n % i == 0)
        {
            sum += i;
        }
    }
    if (sum == n)
    {
        return 1;
    }
    return 0;
}

void *thr_func(void *param)
{
    THR *p = (THR *)param;

    sem_wait(&sem);

    for (int i = p->start; i <= p->end; i++)
    {
        if (perfect(i))
        {
            result[cnt++] = i;
        }
    }
    sem_post(&sem);

    return NULL;
}

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("nr incorect de argumente\n");
        return 0;
    }
    sem_init(&sem, 0, 1);
    int n = 0, m = 0;

    sscanf(argv[1], "%d", &n);
    sscanf(argv[2], "%d", &m);

    printf("%d %d\n", n, m);

    THR param[100];
    pthread_t tid[100];
    result = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < m; i++)
    {
        param[i].id = i;
        if (i == 0)
        {
            param[i].start = 0;
        }
        else
        {
            param[i].start = param[i - 1].end + 1;
        }
        param[i].end = param[i].start + n / m - 1;
        if (i < n % m)
        {
            param[i].end++;
        }
        pthread_create(&tid[i], NULL, thr_func, &param[i]);
    }

    for (int i = 0; i < m; i++)
    {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < cnt; i++)
    {
        printf("%d ", result[i]);
    }
    printf("\n");
}