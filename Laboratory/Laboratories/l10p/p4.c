#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define NT 23
#define N 4

typedef enum TYPE_S
{
    U,
    CFR
} TYPE_S;

typedef struct THR
{
    int id;
    TYPE_S type;
    // int in_boat;
} THR;

typedef struct BOAT
{
    int nb_U;
    int nb_CFR;
    int total;
} BOAT;

BOAT boats[NT]; // maxim NT/4 //todo un macro or something

pthread_mutex_t lock_boarding;
pthread_mutex_t lock_full;
pthread_mutex_t lock;

pthread_cond_t cond_boarding;
pthread_cond_t cond_full;
pthread_cond_t cond_leave;

int boat_index;
int total_U, total_CFR;
int full;

int not_enough = 0;

int is_safe(TYPE_S type, int bi)
{

    if (boats[bi].nb_U == 3 && type == CFR)
    {
        return 0;
    }

    if (boats[bi].nb_CFR == 3 && type == U)
    {
        return 0;
    }

    if (boats[bi].nb_U == 2 && type == U && (boats[bi].nb_CFR == 1 || total_U - 3 == 0))
    {
        return 0;
    }
    if (boats[bi].nb_CFR == 2 && type == CFR && (boats[bi].nb_U == 1 || total_CFR - 3 == 0))
    {
        return 0;
    }

    if (type == CFR && boats[bi].nb_CFR == 0 && total_CFR - 1 == 0)
    {
        return 0;
    }

    if (type == U && boats[bi].nb_U == 0 && total_U - 1 == 0)
    {
        return 0;
    }

    return 1;
}

int can_leave()
{

    if (total_CFR + total_U < N) //|| (total_CFR + total_U == N && (total_U == 3 || total_CFR == 3)))
    {

        return 0;
    }

    return 1;
}

void boarding(THR *t)
{

    pthread_mutex_lock(&lock_boarding);

    while (((boats[boat_index].total + 1 > N) || is_safe(t->type, boat_index) == 0) && can_leave() == 1)
    {
        pthread_cond_wait(&cond_boarding, &lock_boarding);
    }

    if (t->type == U)
    {
        boats[boat_index].nb_U++;
    }
    else
    {
        boats[boat_index].nb_CFR++;
    }

    boats[boat_index].total = boats[boat_index].nb_U + boats[boat_index].nb_CFR;

    if (boats[boat_index].total == N)
    {

        full = 1;
    }
    else if (can_leave() == 0)
    {
        pthread_cond_broadcast(&cond_full);
    }

    pthread_mutex_unlock(&lock_boarding);
}

void full_boat(THR *t)
{
    pthread_mutex_lock(&lock_full);

    while (full == 0 && can_leave() == 1)
    {
        printf("[BOAT %d] is not full... %d %d %d %d\n", boat_index, boats[boat_index].nb_CFR, boats[boat_index].nb_U, total_CFR, total_U);

        pthread_cond_wait(&cond_full, &lock_full);
    }

    if (full == 1)
    {
        printf("[FULL] Boat %d is full. Supporters: U = %d ~~ CFR = %d\n", boat_index, boats[boat_index].nb_U, boats[boat_index].nb_CFR);
        full = 0;
        total_CFR -= boats[boat_index].nb_CFR;
        total_U -= boats[boat_index].nb_U;
        boat_index++;
        pthread_cond_broadcast(&cond_boarding);
    }
    if (can_leave() == 0)
    {
        //printf("--------------no suppoerters!\n");
        pthread_cond_broadcast(&cond_full);
    }
    pthread_mutex_unlock(&lock_full);
}

void *thr_func(void *a)
{
    THR *t = (THR *)a;

    boarding(t);
    usleep(1000);
    full_boat(t);
    return NULL;
}

int main()
{

    pthread_t tid[NT];
    THR params[NT];

    for (int i = 0; i < NT; i++)
    {
        params[i].id = i;
        params[i].type = i % 2;
        if (i % 2 == 0)
        {
            total_U++;
        }
        else
        {
            total_CFR++;
        }
    }
    for (int i = 0; i < NT; i++)
    {

        pthread_create(&tid[i], NULL, thr_func, &params[i]);
    }

    for (int i = 0; i < NT; i++)
    {
        pthread_join(tid[i], NULL);
    }
}