#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_NAME_RESP "RESP_PIPE_38391"
#define FIFO_NAME_REQ "REQ_PIPE_38391"
#define fifo_test "req_test"

char begin_string_field[7];

void set_success()
{
    begin_string_field[0] = 'B';
    begin_string_field[1] = 'E';
    begin_string_field[2] = 'G';
    begin_string_field[3] = 'I';
    begin_string_field[4] = 'N';
    begin_string_field[5] = '.';
}

int main()
{
    int fd_resp = -1;
    int fd_req = -1;

    fd_req = open(fifo_test, O_WRONLY);

    if (fd_req == -1)
    {
        printf("aici!2");
        printf("ERROR\ncannot open the request pipe\n");
        return 1;
    }

    fd_resp = open(FIFO_NAME_RESP, O_RDONLY);

    if (fd_resp == -1)
    {
        printf("aici!3");

        printf("ERROR\ncannot create the response pipe\n");
        return 1;
    }

    char a;
    do
    {
        read(fd_resp, &a, sizeof(a));
        printf("verify: %c\n", a);
    } while (a != '.');

    printf("verify SUCCESS\n");

    char s[6] = "PING.";
    s[6] = 0;

    printf("%s\n", s);
    int index = 0;
    while (index < 5)
    {
        printf("[verify] %c\n",s[index]);
        write(fd_req, &s[index], sizeof(s[index]));
        index++;
    }
}