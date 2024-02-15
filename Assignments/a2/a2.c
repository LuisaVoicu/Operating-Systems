#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include "a2_helper.h"

#define NB_PROCESSES 9

pid_t fork(void);
pid_t getpid(void);
pid_t getppid(void);

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

typedef struct _THR
{
    int P;
    int T;
    int started;
    int ended;

    pthread_mutex_t *lock1;
    pthread_mutex_t *lock2;
    pthread_cond_t *cond_count;
    pthread_cond_t *spec_cond;

    pthread_cond_t *cond_started;
    pthread_cond_t *cond_ended;
    pthread_cond_t *cond_t14_started;

    sem_t *sem_t34_end;
    sem_t *sem_t41_end;
} THR;

sem_t sem_p3_end, sem_p3_start, sem_p3;
sem_t sem_p8_four, sem_p8_t14, sem_all_p8, sem_p8;

sem_t sem_p4, sem_p4_end_thr;
sem_t sem_t34_create;

int count_threads_p8;
int p8_t14_cond;
int found_t14;
int count_all_p8;
int count_finished_p4 = 0;

void *thr_func(void *p)
{
    THR *t = (THR *)p;
    info(BEGIN, t->P, t->T);
    info(END, t->P, t->T);
    return NULL;
}

void *thr_func_p3(void *p)
{

    THR *t = (THR *)p;

    if (t->T == 3)
    {
        info(BEGIN, t->P, t->T);
        sem_post(&sem_p3_start); // deschid  pentru prima data semaforul
    }
    else
    {
        sem_wait(&sem_p3_start);

        if (t->T == 4)
        {
            //  sleep(1);
            sem_wait(t->sem_t41_end);
            info(BEGIN, t->P, t->T);
        }
        else
        {
            info(BEGIN, t->P, t->T);
        }

        sem_post(&sem_p3_start);
    }

    if (t->T == 1)
    {
        info(END, t->P, t->T);
        sem_post(&sem_p3_end); // deschid semaforul pentru a permite afisarea mesajului de sfarstit a thread-ului 3
    }
    else
    {
        sem_wait(&sem_p3_end);
        info(END, t->P, t->T);

        if (t->T == 4)
        {
            sem_post(t->sem_t34_end);
        }

        sem_post(&sem_p3_end);
    }

    return NULL;
}

int ended_t14 = 0;
int special_case = 0;
int sterted_t14;
void *thr_func_p8(void *p)
{
    THR *t = (THR *)p;

    sem_wait(&sem_p8_four);

    info(BEGIN, t->P, t->T);
    printf(" -----> %d %d --> %d\n", t->P, t->T, count_all_p8);

    pthread_mutex_lock(t->lock1);
    count_threads_p8++;

    if (t->T == 14)
    {
        found_t14 = 1;
        sterted_t14 = 1;
    }
    pthread_cond_signal(t->cond_count);
    pthread_mutex_unlock(t->lock1);
    // printf("~~~~~~~~~~~~~~~~~~~~~~~~`````` %d - cnt %d - > rest %d\n", t->T, count_threads_p8, count_all_p8);

    pthread_mutex_lock(t->lock2);

    // cand am t14 -- cazuri : 1) se afla printre primele 44 de numere -- o sa astept sa am 4 thread-uri simulan(doar aici)
    //                         2) se afla printre ultimele 4 si intra in bucla daca folosesc metoda de la 1) --> deci fortez pe t14 sa se termine
    if (found_t14 == 1 && count_all_p8 > 4)
    {

        while (count_threads_p8 != 4 && found_t14 == 1) //&& count_all_p8 >= 4
        {
            pthread_cond_wait(t->cond_count, t->lock2);
        }

        if (ended_t14 == 0)
        {

            info(END, t->P, 14);
            ended_t14 = 1;
        }

        found_t14 = 0;
    }
    else if (count_all_p8 <= 4)
    {

        if (sterted_t14 == 0)
        {
            special_case = 1;
            // pthread_cond_broadcast(t->spec_cond);
        }
        else
        {
            special_case = 0;
            if (ended_t14 == 0)
            {
                info(END, t->P, 14);
                ended_t14 = 1;
            }
            pthread_cond_broadcast(t->spec_cond);
        }
    }

    pthread_mutex_unlock(t->lock2);

    pthread_mutex_lock(t->lock1);
    count_threads_p8--;
    count_all_p8--;
    pthread_cond_signal(t->cond_count);
    pthread_mutex_unlock(t->lock1);

    if (t->T != 14)
    {

        pthread_mutex_lock(t->lock1);

        while (special_case != 0)
        {
            pthread_cond_wait(t->spec_cond, t->lock1);
        }
        info(END, t->P, t->T);

        pthread_mutex_unlock(t->lock1);
    }
    else
    {
        printf("\n----aici e 14~~~~~~~~~ \n");
        //  info(END, t->P, t->T);
    }
    sem_post(&sem_p8_four);

    return NULL;
}

void *thr_func_p4(void *p)
{

    THR *t = (THR *)p;

    if (t->started == 0)
    {
        if (t->T != 3)
        {
            info(BEGIN, t->P, t->T);
            t->started = 1;
        }
        else if (t->T == 3)
        {

            sem_wait(t->sem_t34_end);
            info(BEGIN, t->P, t->T);
            t->started = 1;
        }
    }

    if (t->started == 1 && t->ended == 0)
    {

        if (t->T == 1)
        {
            sem_post(t->sem_t41_end);
            info(END, t->P, t->T);
            count_all_p8++;
        }
        else
        {
            info(END, t->P, t->T);
            count_all_p8++;
        }

        t->ended = 1;
        count_finished_p4++;
        sem_post(&sem_p4_end_thr);
    }

    return NULL;
}

int main()
{
    init();

    pid_t pidP2 = -1;
    pid_t pidP3 = -1;

    // pthread_t tidP4[7];
    // THR params[7];
    sem_t *my_second_sem = NULL;
    sem_t *my_sem = NULL;

    sem_unlink("so_a2_sem");
    sem_unlink("so_a2_sem_t41");

    my_sem = sem_open("so_a2_sem", O_CREAT, 0644, 0);
    my_second_sem = sem_open("so_a2_sem_t41", O_CREAT, 0644, 0);

    info(BEGIN, 1, 0);

    sem_init(my_sem, 1, 0);
    sem_init(my_second_sem, 1, 0);

    int val;
    sem_getvalue(my_sem, &val);
    printf(" ------------------------ [main] %d --\n", val);

    pidP2 = fork();
    if (pidP2 == 0)
    {
        info(BEGIN, 2, 0);
        info(END, 2, 0);
    }
    else if (pidP2 > 0)
    {
        pidP3 = fork();
        if (pidP3 == 0)
        {

            info(BEGIN, 3, 0);
            // info(BEGIN, 4, 0);
            // sem_init(my_sem,0,0);
            pthread_t tidP4[7];

            pid_t pidP4 = -1, pidP8 = -1, pidP9 = -1;
            pidP4 = fork();
            if (pidP4 == 0)
            {
                info(BEGIN, 4, 0);
                pid_t pidP5 = -1, pidP6 = -1, pidP7 = -1;
                pidP5 = fork();
                if (pidP5 == 0)
                {
                    info(BEGIN, 5, 0);

                    info(END, 5, 0);
                }
                else
                {
                    pidP6 = fork();
                    if (pidP6 == 0)
                    {
                        info(BEGIN, 6, 0);

                        info(END, 6, 0);
                    }
                    else
                    {
                        pidP7 = fork();
                        if (pidP7 == 0)
                        {
                            info(BEGIN, 7, 0);

                            info(END, 7, 0);
                        }
                        else
                        {

                            //  process 4
                            waitpid(pidP5, NULL, 0);
                            waitpid(pidP6, NULL, 0);
                            waitpid(pidP7, NULL, 0);
                            // pthread_t tidP4[7];
                            THR params[7];

                            sem_init(&sem_p4, 0, 1);

                            sem_init(&sem_p4_end_thr, 0, 0);
                            count_finished_p4 = 0;

                            for (int i = 0; i < 6; i++)
                            {
                                params[i].started = params[i].ended = 0;
                                params[i].P = 4;
                                params[i].T = i + 1;
                                params[i].sem_t34_end = my_sem;
                                params[i].sem_t41_end = my_second_sem;

                                pthread_create(&tidP4[i], NULL, thr_func_p4, &params[i]);
                            }

                            for (int i = 0; i < 6; i++)
                            {
                                pthread_join(tidP4[i], NULL);
                            }

                            info(END, 4, 0);
                        }
                    }
                }
            }
            else
            {
                pidP8 = fork();
                if (pidP8 == 0)
                {
                    // process 8

                    info(BEGIN, 8, 0);

                    sem_init(&sem_p8_four, 0, 4);
                    pthread_cond_t cond_count = PTHREAD_COND_INITIALIZER;
                    pthread_cond_t spec_cond = PTHREAD_COND_INITIALIZER;
                    pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
                    pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

                    pthread_t tidP8[49];
                    THR params[49];
                    int n = 48;
                    count_all_p8 = n;
                    count_threads_p8 = 0;
                    found_t14 = 0;
                    ended_t14 = 0;

                    for (int i = 0; i < n; i++)
                    {
                        params[i].P = 8;
                        params[i].T = i + 1;
                        params[i].started = params[i].ended = 0;
                        params[i].cond_count = &cond_count;
                        params[i].spec_cond = &spec_cond;
                        params[i].lock1 = &lock1;
                        params[i].lock2 = &lock2;
                        pthread_create(&tidP8[i], NULL, thr_func_p8, &params[i]);
                    }

                    for (int i = 0; i < n; i++)
                    {
                        pthread_join(tidP8[i], NULL);
                    }

                  

                    info(END, 8, 0);
                }
                else
                {
                    pidP9 = fork();
                    if (pidP9 == 0)
                    {
                        // process 9
                        info(BEGIN, 9, 0);

                        info(END, 9, 0);
                    }
                    else
                    {
                        // process 3

                        waitpid(pidP8, NULL, 0);
                        waitpid(pidP9, NULL, 0);

                        sem_init(&sem_p3, 0, 1);

                        pthread_t tidP3[5];
                        THR paramsP3[5];
                        sem_init(&sem_p3_end, 0, 0);
                        sem_init(&sem_p3_start, 0, 0);

                        for (int i = 0; i < 4; i++)
                        {
                            paramsP3[i].P = 3;
                            paramsP3[i].T = i + 1;
                            paramsP3[i].sem_t34_end = my_sem;
                            paramsP3[i].sem_t41_end = my_second_sem;
                            pthread_create(&tidP3[i], NULL, thr_func_p3, &paramsP3[i]);
                        }

                        for (int i = 0; i < 4; i++)
                        {
                            pthread_join(tidP3[i], NULL);
                        }
                        waitpid(pidP4, NULL, 0);

                        info(END, 3, 0);
                    }
                }
            }
        }
        else if (pidP3 > 0)
        {
            // 41
            waitpid(pidP2, NULL, 0);
            waitpid(pidP3, NULL, 0);
            info(END, 1, 0);
        }
    }

    return 0;
}