#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t p2, p3, p4;

    p2 = fork();

    if (p2 == 0)
    {
        // p3 porneste abia dupa ce p4 s-a terminat --> controlez din waitpid
        p4 = fork();
        if (p4 == 0)
        {
            printf("I'm process 4\n");
            exit(4);
        }
        else
        {
            int status = 0;
            waitpid(p2, &status, 0);
            printf("Process 4 finished with status %d\n", WEXITSTATUS(status));

            p3 = fork();
            if (p3 == 0)
            {
                printf("I'm process 3\n");
                exit(3);
            }
            else
            {

                waitpid(p3, &status, 0);
                printf("Process 3 finished with status %d\n", WEXITSTATUS(status));

                printf("I'm process 2\n");
                exit(2);
            }
        }
    }

    else
    {
        int status = 0;
        waitpid(p2, &status, 0);
        printf("Process 2 finished with status %d\n", WEXITSTATUS(status));
        printf("I'm process 1\n");
    }
}