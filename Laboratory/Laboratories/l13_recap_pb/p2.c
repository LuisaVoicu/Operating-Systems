#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

typedef struct THR
{
    char *s;
} THR;

int vowelCount = 0;
sem_t sem;

void *thr_func(void *p)
{
    THR *word = (THR *)p;
    printf("%s\n", word->s);
    sem_wait(&sem);

    for (int i = 0; i < strlen(word->s); i++)
    {
        if (strchr("aeiouAEIOU", word->s[i]))
        {
            vowelCount++;
        }
    }
    sem_post(&sem);

    return NULL;
}
int main(int argc, char **argv)
{
    sem_init(&sem, 0, 1);
    if (argc < 2)
    {
        printf("nu sunt suficiente argumente");
        return 1;
    }

    int n = 0;

    sscanf(argv[1], "%d", &n);
    pthread_t tid[n];
    THR param[n];

    for (int i = 0; i < n; i++)
    {
        strcpy(param[i].s, argv[2 + i]);
        pthread_create(&tid[i], NULL, thr_func, &param[i]);
    }

    for (int i = 0; i < n; i++)
    {
        pthread_join(tid[i], NULL);
    }

    printf("nb vowels = %d\n",vowelCount);
}