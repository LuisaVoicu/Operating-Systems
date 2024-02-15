#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define FIFO_NAME "l13_fifo"

int list_dir(char *path, int fd_fifo)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char new_path[512];
    struct stat stat_buf;

    dir = opendir(path);
    if (dir == NULL)
    {
        printf("directorul nu poate fi deschis\n");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(new_path, 512, "%s/%s", path, entry->d_name);
        if (lstat(new_path, &stat_buf) == 0)
        {
            if (S_ISREG(stat_buf.st_mode))
            {
                printf("%s\n", entry->d_name);
                write(fd_fifo, entry->d_name, 100);
            }
        }
    }
    closedir(dir);
    return 0;
}

int main()
{
    unlink(FIFO_NAME);

    if (mkfifo(FIFO_NAME, 0600) != 0)
    {
        printf("fifo nu poate fi creat.\n");
        return 1;
    }
    int fd_fifo = -1;
    fd_fifo = open(FIFO_NAME, O_WRONLY);
    if (fd_fifo == -1)
    {
        printf("pipe-ul nu poate fi deschis in p1");
        return 1;
    }
    char path[200] = "/home/luisa/Desktop/l13_recap_pb/p7_dir";
    if (list_dir(path, fd_fifo) != 0)
    {
        printf("folderul nu se poate deschide!\n");
    }

    close(fd_fifo);
    unlink(FIFO_NAME);
    return 0;
}