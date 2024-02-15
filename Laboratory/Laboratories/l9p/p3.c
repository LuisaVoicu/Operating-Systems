#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#define N 15

typedef struct
{
    int id;
    char path[20];
    int fdi;

} THR;

int count_open;

pthread_mutex_t lock;
pthread_mutex_t lock2;
pthread_mutex_t lock_turn;
pthread_cond_t cond_turn;
pthread_cond_t cond_fd;
int turn;

THR params[N];

int fdi = 0;
int open_once(const char *path)
{
    pthread_mutex_lock(&lock2);

    int not_open = 1;
    for (int i = 0; i < N; i++)
    {
        if (strcmp(path, params[i].path) == 0 && params[i].fdi != -1)
        {
            fdi = params[i].fdi;
            not_open = 0;
            break;
        }
    }
    if (not_open == 1)
    {
        printf("opening %s .... \n", path);
        fdi = open(path, 0644);
    }
    pthread_mutex_unlock(&lock2);
    return fdi;
}

void *thr_func(void *a)
{
    THR *s = (THR *)a;
    pthread_mutex_lock(&lock_turn);
    while (s->id != turn)
    {
        pthread_cond_wait(&cond_turn, &lock_turn);
    }

    int fd = open_once(s->path);
    char buf = 0;
    s->fdi = fdi;
    ssize_t size = read(fdi, &buf, 1);

    printf("[THR %d] %d %d-->", s->id, turn, fd);
    if (size > 0)
    {
        printf(":%c\n ", buf);
    }
    else if (size == 0)
    {
        printf("eof\n");
    }
    else
    {
        printf("nu se poate citi din fisier!\n");
    }

    turn++;
    pthread_cond_broadcast(&cond_turn);

    pthread_mutex_unlock(&lock_turn);
    return NULL;
}

int main()
{

    pthread_t pid[N];

    char path[20] = "a.txt";
    path[9] = 0;

    char path2[20] = "b.txt";
    path2[9] = 0;

    char path3[20] = "c.txt";
    path3[9] = 0;

    for (int i = 0; i < N; i++)
    {
        params[i].id = i;
        params[i].fdi = -1;

        if (i % 3 == 0)
        {
            strcpy(params[i].path, path);
        }
        else if (i % 3 == 1)
        {
            strcpy(params[i].path, path2);
        }
        else
        {
            strcpy(params[i].path, path3);
        }
    }

    for (int i = 0; i < N; i++)
    {
        pthread_create(&pid[i], NULL, thr_func, &params[i]);
    }

    for (int i = 0; i < N; i++)
    {
        //  close(params[i].fdi);
        pthread_join(pid[i], NULL);
    }
}