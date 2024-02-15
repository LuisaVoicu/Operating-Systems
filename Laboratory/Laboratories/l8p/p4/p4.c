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
#define N 13

typedef struct
{
    int tip;
    int x;
    int y;
} THR;

sem_t sem;
sem_t sem_tr;
sem_t sem_done;

int prevTip;
THR *prevS;
int cancel_index;
void *func(void *p)
{
    THR *s = (void *)p;

    for (;;)
    {

        int tip = (int)(long)s->tip;
        sleep(rand() % 5);
        int id;
        if (s->tip == 0)
        {
            id = s->x;
        }
        else
        {
            id = s->y;
        }
        printf("im %d\n", id);

        sem_wait(&sem_tr);

        // verific daca primesc thread de tip diferit pentru a forma molecula
        if (prevTip != tip)
        {
            // daca
            if (prevS != NULL)
            {

                // daca tipul e diferit il retin si formez molecula
                if (s->tip == 0 && s->y == 0 && prevS->x == 0)
                {
                    s->y = prevS->y;
                    prevS->x = s->x;
                    printf("------------------------------>>>>>>>>>> found: %d %d\n", s->x, s->y);
                    sem_post(&sem); // deblochez semaforul
                }
                else if (s->tip == 1 && s->x == 0 && prevS->y == 0)
                {
                    s->x = prevS->x;
                    prevS->y = s->y;
                    printf("------------------------------>>>>>>>>>> found: %d %d\n", s->x, s->y);
                    sem_post(&sem);
                }
            }
        }
        else
        {
            prevTip = s->tip;
            prevS = s;
        }

        sem_post(&sem_tr);
    }
    return NULL;
}

int is_done(THR t[])
{
    sem_wait(&sem_done);
    for (int i = 0; i < N; i++)
    {
        // printf("++++++++++++++++++++++++++++ %d: %d %d %d\n", i + 1, t[i].tip, t[i].x, t[i].y);

        if (t[i].x == 0 || t[i].y == 0)
        {
            for (int j = i + 1; j < N; j++)
            {
                // printf("++++++++++++++++++++++++++++++++%d: %d %d %d\n", j + 1, t[j].tip, t[j].x, t[j].y);
                if (t[i].tip != t[j].tip && (t[j].x == 0 || t[j].y == 0))
                {
                    return 0;
                }
            }
        }
    }
    // printf("not found!!!!!!!!!!!!!!!!!!!!!!!!!!!1\n");
    return 1;
}

int main()
{

    pthread_t tid[N + 1];
    THR t[N + 1];
    sem_init(&sem, 0, 0);
    sem_init(&sem_tr, 0, 1);
    sem_init(&sem_done, 0, 0);

    for (int i = 0; i < N; i++)
    {
        t[i].tip = rand() % 2; // Na sau Cl

        if (t[i].tip == 0)
        {
            t[i].x = i + 1;
            t[i].y = 0;
        }
        else
        {
            t[i].x = 0;
            t[i].y = i + 1;
        }
        pthread_create(&tid[i], NULL, func, &t[i]);
    }

    for (;;)
    {

        sem_wait(&sem);
        for (int i = 0; i < N; i++)
        {
            // printf("%d: %d %d %d\n", i + 1, t[i].tip, t[i].x, t[i].y);
            if (t[i].x != 0 && t[i].y != 0)
            {
                pthread_cancel(tid[t[i].x - 1]);
                pthread_cancel(tid[t[i].y - 1]);
                printf("formeaza molecula %d %d \n", t[i].x, t[i].y);
            }
        }
        sleep(1);
        sem_post(&sem_done);
        if (is_done(t) == 1)
        {
            // inchid si restul thread-urilor
            for (int i = 0; i < N; i++)
            {
                pthread_cancel(tid[i]);
            }
            break;
        }
    }

    /*    for (int i = 0; i < N; i++)
       {
           if (t[i].x != 0 && t[i].y != 0)
           {
               int cl = t[i].x + 1;
               int na = t[i].y + 1;
               if (t[i].tip == 1)
               {
                   cl = t[i].y + 1;
                   na = t[i].x + 1;
               }
               printf("Na=%d; Cl=%d\n", na, cl);
           }
       } */
    for (int i = 0; i < N; i++)
    {
        pthread_join(tid[i], NULL);
    }
    for (int i = 0; i < N; i++)
    {
        if (t[i].x != 0 && t[i].y != 0)
        {
            int cl = t[i].x + 1;
            int na = t[i].y + 1;
            if (t[i].tip == 1)
            {
                cl = t[i].y + 1;
                na = t[i].x + 1;
            }
            printf("Na=%d; Cl=%d\n", na, cl);
        }
        else
        {
            if (t[i].tip == 0)
            {
                printf("Na cu id-ul %d nu formeaza molecula\n",t[i].x);
            }
            else
            {
                printf("Cl cu id-ul %d nu formeaza molecula\n",t[i].y);
            }
        }
    }

    sem_destroy(&sem);
}