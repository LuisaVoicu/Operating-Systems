#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#define N 3

typedef struct
{
    int id;
    sem_t *sem;
} THR;

int expected;
/* void *thr_func(void *p)
{
    THR *s = (void *)p;

    int id = (int)(long)s->id;
    for (;;)
    {
        sem_wait(s->sem);
        if (expected == id)
        {
            printf("%d ", id + 1);
            expected = (expected == N - 1) ? 0 : (expected + 1);
        }

        usleep(rand()%10000+5000);
        //sleep(1);
        sem_post(s->sem);
        usleep(rand()%10000+5000);

    }
    return NULL;
} */

int ant;
void *thr_func(void *p)
{
    THR *s = (void *)p;

    int id = (int)(long)s->id;
    for (;;)
    {
        // printf("++ %d\n", id + 1);

        //  sem_wait(s->sem);
        // sem_post(s->sem);

        // usleep(rand()%5000+1000);
        sleep(rand() % 5);
        //printf("!!! %d %d %d\n", ant, id + 1, expected);
        
        int expected = (ant == N) ? 1 : (ant + 1);
        if (expected == id + 1)
        {
            // activez semaforul
            printf("-- %d\n", id + 1);
            ant = id + 1;
            sem_post(s->sem);
        }
        else
        {

            int val;
            sem_getvalue(s->sem, &val);
            // blochez semanul pana la aparitia nr coreps (daca e 0 nu fac nimic)
            if (val != 0)
            {
                sem_wait(s->sem);
            }
        }

        // sem_post(s->sem);
    }
    return NULL;
}

int main()
{
    pthread_t tid[N + 1];
    THR t[N + 1];
    sem_t sem;
    sem_init(&sem, 0, 1);
    for (int i = 0; i < N; i++)
    {
        t[i].id = i;
        t[i].sem = &sem;
        pthread_create(&tid[i], NULL, thr_func, &t[i]);
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(tid[i], NULL);
    }
    sem_destroy(&sem);
}