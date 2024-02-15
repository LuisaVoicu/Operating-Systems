#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#define N 4
#define NT 6
typedef struct
{
    int dir;
    int id;
} THR;

pthread_mutex_t lock_enter;
pthread_mutex_t lock_exit;
pthread_mutex_t lock_cross;
pthread_cond_t cond_ent;
pthread_cond_t cond_ext;
pthread_cond_t cond_dir;

int count;
int dir_init;

void enter_bridge(int dir, int id)
{

    // sleep(1);
    pthread_mutex_lock(&lock_enter);

    while (count + 1 > N)
    {
        printf("Podul e la capacitate maxima!\n");
        pthread_cond_wait(&cond_ent, &lock_enter);
    }

    while (dir != dir_init)
    {
        printf("Directie necorespunzatoare!\n");
        pthread_cond_wait(&cond_dir, &lock_enter);
    }
    printf("hello??\n");
    count++;
    printf("[IN %d] o masina a intrat pe pod; nr masini aflate pe pod = %d; DIR = %d\n", id, count, dir);
    pthread_mutex_unlock(&lock_enter);
}

void cross_bridge(int dir)
{

    pthread_mutex_lock(&lock_cross);
    if (count == 0)
    {
        printf("init aici!!!!!!!!!!!!!\n");
        dir_init = dir;
        pthread_cond_broadcast(&cond_dir);
    }
    pthread_mutex_unlock(&lock_cross);

    // sleep(1);
    pthread_mutex_lock(&lock_cross);
    if (count + 1 <= N)
    {
        pthread_cond_broadcast(&cond_ent);
    }

    if (count - 1 >= 0)
    {
        pthread_cond_broadcast(&cond_ext);
    }
    pthread_mutex_unlock(&lock_cross);
}

void exit_bridge(int dir, int id)
{

    // sleep(1);
    pthread_mutex_lock(&lock_exit);
    while (count - 1 < 0)
    {
        printf("Pe Pod nu se afla nici o masina!\n");
        pthread_cond_wait(&cond_ext, &lock_exit);
    }
    count--;
    printf("[OUT %d] o masina a parasit podul; nr masini aflate pe pod = %d DIR= %d\n", id, count, dir);
    pthread_cond_broadcast(&cond_ent);

    pthread_mutex_unlock(&lock_exit);
}

void *car_thread(void *direction)
{
    THR *dir = (THR *)direction;

    enter_bridge(dir->dir, dir->id);
    cross_bridge(dir->dir);
    exit_bridge(dir->dir, dir->id);

    return NULL;
}

int main()
{
    pthread_cond_init(&cond_dir, NULL);
    pthread_cond_init(&cond_ent, NULL);
    pthread_cond_init(&cond_ext, NULL);
    pthread_mutex_init(&lock_exit, NULL);
    pthread_mutex_init(&lock_enter, NULL);
    pthread_mutex_init(&lock_cross, NULL);

    pthread_t tid[NT];
    THR dir[NT];
    dir_init = 0;
    count = 0;
    srand(time(NULL));

    for (int i = 0; i < NT; i++)
    {
        dir[i].dir = rand() % 2;
        dir[i].id = i;
        printf("%d %d \n", i, dir[i].dir);
    }

    for (int i = 0; i < NT; i++)
    {
        pthread_create(&tid[i], NULL, car_thread, &dir[i]);
    }

    for (int i = 0; i < NT; i++)
    {
        pthread_join(tid[i], NULL);
    }
}