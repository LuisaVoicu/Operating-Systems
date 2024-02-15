#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define NR_CUSTOMERS 20
#define NR_CHAIRS 3

pthread_mutex_t lock;
pthread_mutex_t done_lock;
pthread_cond_t barber;
pthread_cond_t customer;
pthread_cond_t cond_done;

int waitingCustomers = 0;
int barberChair = 0;
int processedClients = 0;

void *th_barber(void *arg)
{
    int crtCustomer = 0;

    pthread_mutex_lock(&done_lock);
    while (processedClients < NR_CUSTOMERS)
    {
        printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~>>>>> %d\n", processedClients);

        pthread_mutex_lock(&lock);
        while (barberChair == 0)
        {
            pthread_cond_wait(&barber, &lock);
        }
        crtCustomer = barberChair;
        pthread_mutex_unlock(&lock);

        printf("[B  ] Serving customer %d\n", crtCustomer);
        usleep(1000);

        pthread_mutex_lock(&lock);
        barberChair = 0;
        pthread_cond_signal(&customer);
        pthread_mutex_unlock(&lock);

        pthread_cond_wait(&cond_done, &done_lock);
    }
    
    pthread_mutex_unlock(&done_lock);

    return NULL;
}

void *th_customer(void *arg)
{
    int myId = (int)(size_t)arg;
    int tooBusy = 0;

    usleep(1000 * (rand() % 20));

    printf("[C%02d] Entering the barber shop\n", myId);

    pthread_mutex_lock(&lock);

    if (waitingCustomers < NR_CHAIRS)
    {
        ++waitingCustomers;
        printf("[C%02d] %d customer(s) waiting\n", myId, waitingCustomers);
        while (barberChair != 0)
        {
            pthread_cond_wait(&customer, &lock);
        }
        --waitingCustomers;
        barberChair = myId;
        pthread_cond_signal(&barber);
    }
    else
    {
        tooBusy = 1;
        printf("[C%02d] Too busy, will come back another day.\n", myId);
    }
    pthread_mutex_unlock(&lock);

    if (!tooBusy)
    {
        printf("[C%02d] being served\n", myId);
        usleep(1000);
    }

    pthread_mutex_lock(&lock);
    processedClients++;
    pthread_cond_signal(&cond_done); // signal
    pthread_mutex_unlock(&lock);

    printf("--------------------- [C%02d] Exiting the barber shop --> client no %d\n", myId, processedClients);

    return NULL;
}

int main()
{
    int i;
    pthread_t tidC[NR_CUSTOMERS], tidB;

    srand(time(NULL));

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&customer, NULL);
    pthread_cond_init(&barber, NULL);

    pthread_create(&tidB, NULL, th_barber, NULL);
    for (i = 0; i < NR_CUSTOMERS; i++)
    {
        pthread_create(&tidC[i], NULL, th_customer, (void *)(size_t)(i + 1));
    }
    for (i = 0; i < NR_CUSTOMERS; i++)
    {
        pthread_join(tidC[i], NULL);
    }
    pthread_join(tidB, NULL);
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~>>> Done~!\n");

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&customer);
    pthread_cond_destroy(&barber);

    return 0;
}