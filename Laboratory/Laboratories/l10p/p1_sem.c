#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define NR_CUSTOMERS 20
#define NR_CHAIRS 3

pthread_mutex_t done_lock;
pthread_mutex_t mutex;
pthread_cond_t cond_done;

sem_t lock;
sem_t customer;
sem_t barber;
sem_t done_client;
int waitingCustomers = 0;
int barberChair = 0;
int count_clients = 0;

void *th_barber(void *arg)
{

    pthread_mutex_lock(&done_lock);
    while (count_clients < NR_CUSTOMERS)
    {
        sem_post(&customer);
        sem_wait(&barber);
        printf("[B  ] Serving customer %d\n", barberChair);
        usleep(1000);
        printf(" [b] ----------> done with %d\n", barberChair);
        sem_post(&done_client);
        pthread_cond_wait(&cond_done, &done_lock);
    }

    printf(" ---  CLIENT NO %d\n" ,count_clients);
    pthread_mutex_unlock(&done_lock);

    return NULL;
}

void *th_customer(void *arg)
{
    int myId = (int)(ssize_t)arg;
    int tooBusy = 0;

    usleep(1000 * (rand() % 20));
    printf("[C%02d] Entering the barber shop\n", myId);

    sem_wait(&lock);
    if (waitingCustomers < NR_CHAIRS)
    {
        ++waitingCustomers;
        printf("[C%02d] %d customer(s) waiting\n", myId, waitingCustomers);
    }
    else
    {
        tooBusy = 1;
        printf("[C%02d] Too busy, will come back another day.\n", myId);
        printf("------------> done with %d\n", myId); // nu ajunge pe scanulul barbierului
        /* pthread_mutex_lock(&mutex);
        count_clients++;
        pthread_cond_signal(&cond_done);
        pthread_mutex_unlock(&mutex); */
    }
    sem_post(&lock);

    if (!tooBusy)
    {
        sem_wait(&customer);
        sem_wait(&lock);
        --waitingCustomers;
        sem_post(&lock);
        barberChair = myId;
        sem_post(&barber);
        printf("[C%02d] being served\n", myId);
        usleep(1000);
        sem_wait(&done_client);
        printf("------------> done with %d\n", myId);
        /*       pthread_mutex_lock(&mutex);
              count_clients++;
              pthread_cond_signal(&cond_done);
              pthread_mutex_unlock(&mutex); */
    }

    pthread_mutex_lock(&mutex);
    count_clients++;
    pthread_cond_signal(&cond_done);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    int i;
    pthread_t tidC[NR_CUSTOMERS], tidB;

    srand(time(NULL));

    sem_init(&lock, 0, 1);
    sem_init(&customer, 0, 0);
    sem_init(&barber, 0, 0);
    sem_init(&done_client, 0, 0);

    pthread_create(&tidB, NULL, th_barber, NULL);
    for (i = 0; i < NR_CUSTOMERS; i++)
    {
        pthread_create(&tidC[i], NULL, th_customer, (void *)(ssize_t)(i + 1));
    }
    for (i = 0; i < NR_CUSTOMERS; i++)
    {
        pthread_join(tidC[i], NULL);
    }
    pthread_cancel(tidB);
    pthread_join(tidB, NULL);

    sem_destroy(&lock);
    sem_destroy(&customer);
    sem_destroy(&barber);

    return 0;
}