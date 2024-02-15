#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define N 5
#define NT 7

typedef struct
{
    int id;
} THR;

pthread_mutex_t lock;
pthread_mutex_t lock_leave;
pthread_cond_t cond_arrive;
pthread_cond_t cond_group;
pthread_cond_t cond_leave;

int count_occ = 0;
int is_group = 0;

void *thr_func(void *s)
{
    THR *t = (THR *)s;

    pthread_mutex_lock(&lock);

    // cat timp sunt clienti la masa
    while (count_occ + 1 > N || is_group == 1)
    {
        printf("[THR %d] WAITING ARRIVE\n", t->id);
        if(is_group == 1)
        printf("There is a group! %d can't join!\n",t->id); 
        printf("\n");
        pthread_cond_wait(&cond_arrive, &lock);
    }

    // stiu ca masa are cel putin un loc liber

    count_occ++;
    if (count_occ == N)
    {
        is_group = 1;
    }
    printf("[THR %d] ARRIVE; TOTAL = %d\n", t->id, count_occ);
    pthread_cond_broadcast(&cond_leave);

    pthread_mutex_unlock(&lock);
    sleep(rand() % 6);

    pthread_mutex_lock(&lock_leave);

    // doar cand cond_occ==0 atunci nu am un grup==> pot sa vina alte persoane in restaurant

    while (count_occ - 1 < 0)
    {
        printf("[THR %d] WAITING LEAVE\n", t->id);
        pthread_cond_wait(&cond_leave, &lock_leave);
    }

    count_occ--;
    if (count_occ == 0)
    {
        is_group = 0;
    }
    printf("[THR %d] LEAVE; TOTAL = %d\n", t->id, count_occ);

    pthread_cond_broadcast(&cond_arrive); // semnalizez ca mai pot ajunge persoane
    pthread_mutex_unlock(&lock_leave);
    return NULL;
}

int main()
{

    pthread_t tid[NT];
    THR params[NT];
    srand(time(NULL));

    for (int i = 0; i < NT; i++)
    {
        params[i].id = i;
        pthread_create(&tid[i], NULL, thr_func, &params[i]);
    }

    for (int i = 0; i < NT; i++)
    {
        params[i].id = i;
        pthread_join(tid[i], NULL);
    }
}