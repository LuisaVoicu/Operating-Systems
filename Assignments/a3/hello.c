#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

void doSomething(int *v, int n)
{

    for (int i = 0; i < n; i++)
    {
        v[i] = i;
    }
}

char* hello(int n)
{
    //char s[10];
    char* ss = (char*)malloc(10);

    return ss;
}
int main()
{
    /*     char s[5];
        strcpy(s, "ping");
        printf("%d", s[5]);

        int *v;
        int n = 3;
       // v = (int *)malloc(n * sizeof(int));

        doSomething(v, n);

        for (int i = 0; i < n; i++)
        {
            printf("%d ", v[i]);
        } */

    /*     int a = 1;
        int n = 4;
        while (n && !fork())
        {
            printf("%d %d -> p: %d mine: %d; ", a, n, getppid(), getpid());
            a++;
            n--;
        }
        printf("\n"); */
/* 
    printf(" init : %d\n", getppid());

    if (fork() && fork() && fork())
    {

        fork();
    }
    else
    {
        printf("%d ```` %d\n", getpid(), getppid());
    } */

 /*    int a=1;
    int b=0,c=2;

    if(a==2||b++||c++)
    {
        printf("results:\n");
    }
    printf("%d %d %d\n",a,b,c); */


 

    //hello(1);



    /* int x=10;



    int pid;


    pid = fork();

  

    if(pid==0)
    {
        x+=2;
    }

    pid = fork();

    printf("sunt aici %d\n",getpid());

    if(pid!=0)
    {
        printf("im here\n");
        x+=3;
    }

    printf("%d %d\n",getpid(),x); */


printf("%d\n",getpid());

for(int i = 0 ; i < 1500000000 ; i++)
{
   if(i%2==0)
    {
        fork();
        sleep(1);
    }

}

    return 0;
}