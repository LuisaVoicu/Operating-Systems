#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#define FUMATOR 3
#define INGREDIENTE 3

typedef struct Smoker
{
    int id;

} Smoker;

typedef struct Dealer
{

} Dealer;

sem_t ingrediente[INGREDIENTE];
sem_t primit;
sem_t sem;
sem_t generat;

int i1, i2;
void *dealer_func(void *p)
{

    for (;;)
    {
        sleep(rand()%4);

        sem_wait(&sem);
        sem_wait(&primit);
        i1 = rand() % 3;
        do
        {
            i2 = rand() % 3;
        } while (i1 == i2);

        printf("ofer: %d %d\n", i1, i2);

        sem_post(&ingrediente[i1]); // semnalizez ca i1 a fost generat
        sem_post(&ingrediente[i2]); // --//--
        sem_post(&generat);         // semnalizez ca ambele numere au fost generate
        sem_post(&sem);             // iesire zona critica
    }
    return NULL;
}

void primeste_ingrediente(int id)
{

    sem_wait(&ingrediente[i2]);
    sem_wait(&ingrediente[i1]);
    printf("--- am primit %d %d\n", i1, i2);
    sem_post(&primit); // spun dealer-ului ca am primit ingredientele si ca mai pot accepta alte 2
                       //(in enunt se precizeaza ca de fiecare daca cand dealer-ul genereaza ingrediente un fumator trebuie sa le accepte)
}

void *smoker_thr(void *p)
{
    Smoker *f = (Smoker *)p;
    for (;;)
    {
        sem_wait(&generat); // nu primesc ingrediente decat daca ambele au fost generate
                            // (altfel apare deadlock --> exemplu: i1 genereaza o data, i2 asteapta sa genereze dar intre timp i1 genereaza alt ingredient)
        primeste_ingrediente(f->id);
    }
    return NULL;
}

int main()
{
    pthread_t TID_Smoker[FUMATOR];
    pthread_t TID_Dealer;
    Smoker params[FUMATOR];
    sem_init(&primit, 0, 1);
    sem_init(&sem, 0, 1);
    sem_init(&generat, 0, 0);

    for (int i = 0; i < INGREDIENTE; i++)
    {
        sem_init(&ingrediente[i], 0, 0);
    }

    srand(time(NULL));
    pthread_create(&TID_Dealer, NULL, dealer_func, NULL);

    params[0].id = 0;
    params[1].id = 1;
    params[2].id = 2;
    pthread_create(&TID_Smoker[0], NULL, smoker_thr, &params[0]);
    pthread_create(&TID_Smoker[1], NULL, smoker_thr, &params[1]);
    pthread_create(&TID_Smoker[2], NULL, smoker_thr, &params[2]);

    for (int i = 0; i < FUMATOR; i++)
    {
        printf("------------- done with %d\n", i + 1);
        pthread_join(TID_Smoker[i], NULL);
    }
}