#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define NR_THREADS 7
#define AMOUNT_DEPOSIT 11
#define AMOUNT_WITHDRAW 7

typedef struct
{
    int id;
    pthread_mutex_t *lock;
    pthread_mutex_t *lock2;
    pthread_cond_t *cond_turn;
    pthread_cond_t *cond_turn2;
    pthread_cond_t *cond;
} TH_STRUCT;

pthread_mutex_t lock_turn[NR_THREADS];

pthread_cond_t cond_turn[NR_THREADS];
pthread_cond_t c;
pthread_mutex_t l;

int balance = 0;
int turn;

int withdraw_turn = 0;
void *thread_withdraw(void *arg)
{

    TH_STRUCT *s = (TH_STRUCT *)arg;

    pthread_mutex_lock(s->lock);

    while (s->id != turn)
    {
        pthread_cond_wait(s->cond_turn, s->lock);
    }


    turn++;
    if (turn == 0 || turn == 2 || turn == 3 || turn == 4)
    {
        pthread_cond_broadcast(s->cond_turn);
    }
    else
    {
        pthread_cond_broadcast(s->cond_turn2);
    }
    printf("------ [W] in regiunea critica: id = %d ------\n", s->id);

    while (balance < AMOUNT_WITHDRAW)
    {
        printf("[Th%d] Not enough money (%d). Will wait...\n", s->id, balance);
        pthread_cond_wait(s->cond, s->lock);
    }
    balance -= AMOUNT_WITHDRAW;
    printf("[Th%d] Withdrawn %d. Current balance is %d.\n", s->id, AMOUNT_WITHDRAW, balance);

    pthread_mutex_unlock(s->lock);
    return NULL;
}

void *thread_deposit(void *arg)
{

    TH_STRUCT *s = (TH_STRUCT *)arg;

    pthread_mutex_lock(s->lock2);

    while (s->id != turn)
    {
        pthread_cond_wait(s->cond_turn2, s->lock2);
    }

    printf("------ [D] in regiunea critica: id = %d ------\n", s->id);

    balance += AMOUNT_DEPOSIT;
    printf("[Th%d] Deposited %d. Current balance is %d.\n",
           s->id, AMOUNT_DEPOSIT, balance);
    if (balance >= 2 * AMOUNT_WITHDRAW)
    {
        pthread_cond_broadcast(s->cond);
    }
    else if (balance >= AMOUNT_WITHDRAW)
    {
        pthread_cond_signal(s->cond);
    }

    turn++;
    if (turn == 0 || turn == 2 || turn == 3 || turn == 4)
    {
        pthread_cond_broadcast(s->cond_turn);
    }
    else
    {
        pthread_cond_broadcast(s->cond_turn2);
    }

    pthread_mutex_unlock(s->lock2);

    return NULL;
}

int main()
{
    int i;
    TH_STRUCT params[NR_THREADS];
    pthread_t tids[NR_THREADS];
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t c = PTHREAD_COND_INITIALIZER;
    pthread_cond_t c2 = PTHREAD_COND_INITIALIZER;

    turn = 0;

    for (i = 0; i < NR_THREADS; i++)
    {
        pthread_mutex_init(&lock_turn[i], NULL);
        pthread_cond_init(&cond_turn[i], NULL);
    }
    for (i = 0; i < NR_THREADS; i++)
    {

        params[i].id = i;
        params[i].lock = &lock;
        params[i].lock2 = &lock2;
        params[i].cond = &cond;
        params[i].cond_turn = &c;
        params[i].cond_turn2 = &c2;
        if (i == 0 || i == 2 || i == 3 || i == 4)
        {
            pthread_create(&tids[i], NULL, thread_withdraw, &params[i]);
        }
        else
        {
            pthread_create(&tids[i], NULL, thread_deposit, &params[i]);
        }
    }
    for (i = 0; i < NR_THREADS; i++)
    {
        pthread_join(tids[i], NULL);
    }
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&lock);

    return 0;
}
