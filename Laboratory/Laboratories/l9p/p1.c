#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define NT 5

typedef struct
{
    int id;
    int count;
    pthread_mutex_t *lock1;
    pthread_mutex_t *lock2;
    pthread_cond_t *cond_used;
} THR;

int *v;
int *nrDiv;
int n;
int ind;

int count_div(int x)
{
    int count = 1; // numar inclusiv x
    for (int i = 2; i < x; i++)
    {
        if (x % i == 0)
        {
            count++;
        }
    }
    return count;
}

int index;

void *thr_func(void *s)
{
    THR *t = (THR *)s;

    int local_nb = 0;
    while (index < n)
    {
        pthread_mutex_lock(t->lock2);
        local_nb = index;
        index++;
        pthread_mutex_unlock(t->lock2);

        nrDiv[local_nb] = count_div(v[local_nb]);
        printf("THR=%d  NB_DIV_INDEX=%d \n", t->id, local_nb);
    }

    return NULL;
}

int main()
{
    pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_used = PTHREAD_COND_INITIALIZER;
    pthread_t tid[NT];
    THR params[NT];
    srand(time(NULL));
    n = 17;
    v = (int *)calloc(n + 1, sizeof(int));
    nrDiv = (int *)calloc(n + 1, sizeof(int));
    for (int i = 0; i < n; i++)
    {
        v[i] = rand() % 100 + 1;
    }
    ind = 0;

    for (int i = 0; i < NT; i++)
    {
        params[i].count = 0;
        params[i].cond_used = &cond_used;
        params[i].id = i + 1;
        params[i].lock1 = &lock1;
        params[i].lock2 = &lock2;
        pthread_create(&tid[i], NULL, thr_func, &params[i]);
    }

    for (int i = 0; i < NT; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_cond_destroy(&cond_used);
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
}