#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_NAME_RESP "RESP_PIPE_38391"
#define FIFO_NAME_REQ "REQ_PIPE_38391"
#define fifo_test "req_test"
#define MEM_SIZE 1696573

int fd_resp = -1;
int fd_req = -1;
int fd_shm = -1;
int fd_file = -1;

int file_size = 0;

char *shared_memo;
char *shared_file;

int wrong_offset(int offset, int no_bytes, int max_size)
{
    if (offset < 0 || offset + no_bytes > max_size || offset > max_size)
        return 1;
    return 0;
}

void write_response_string(char resp[])
{
    int index = 0;

    do
    {

        write(fd_resp, &resp[index], sizeof(resp[index]));
        index++;
    } while (resp[index - 1] != '!');
}
void write_response_number(int nb)
{
    int index = 0;
    while (index < 4)
    {
        int wr = nb % (16 * 16);
        nb /= (16 * 16);
        write(fd_resp, &wr, 1);
        index++;
    }
}

char *read_request_string()
{
    char a;
    char *s = (char *)calloc(251, sizeof(char));
    int i = 0;
    do
    {
        read(fd_req, &a, sizeof(a));
        s[i++] = a;
  

    } while (a != '!');

    s[i - 1] = 0;
    return s;
}

int read_request_number()
{
  
    int a;
    int index = 0;
    int pow_of_16 = 1;
    int result = 0;
    while (index < 4)
    {
        a = 0;
        read(fd_req, &a, 1);
        result += a * pow_of_16;
        pow_of_16 *= 16 * 16;
        index++;
    }
    return result;
}

void do_ping(char *s)
{
    unsigned int nb = 38391;
    write_response_string("PING!");
    write_response_string("PONG!");
    write_response_number(nb);
}

void do_create_shm(int size)
{

    int message = 1;

    fd_shm = shm_open("/wpaGnwVu", O_CREAT | O_RDWR, 0664);
    if (fd_shm < 0)
    {
        message = 0;
        goto write_message;
    }
    ftruncate(fd_shm, size);

    // mapare todo:
    shared_memo = (char *)mmap(0, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);

    if (shared_memo == (void *)-1)
    {
        message = 0;
        shm_unlink("/wpaGnwVu");
        goto write_message;
    }

write_message:
    write_response_string("CREATE_SHM!");
    if (message == 1)
    {
        write_response_string("SUCCESS!");
    }
    else
    {
        write_response_string("ERROR!");
    }
}

void do_write_to_shm(unsigned int offset, unsigned int value)
{
    int message = 1;
    //  char *put_sm = NULL;
    shared_memo = NULL;

    if (wrong_offset(offset, 4, MEM_SIZE) == 1)
    {
        message = 0;
        goto write_message;
    }



    if (fd_shm == -1 || shared_memo == (void *)-1)
    {
        message = 0;
        goto write_message;
    }

    lseek(fd_shm, offset, SEEK_SET);
    write(fd_shm, &value, 4); // todo aici nu e ok

write_message:

    shared_memo = NULL;

    write_response_string("WRITE_TO_SHM!");
    if (message == 1)
    {
        write_response_string("SUCCESS!");
    }
    else
    {
        write_response_string("ERROR!");
    }
}

void do_map_file(char *path)
{

    int message = 1;
    fd_file = open(path, O_RDONLY);
    if (fd_file == -1)
    {
        message = 0;
        goto write_message;
    }

    file_size = lseek(fd_file, 0, SEEK_END);
    lseek(fd_file, 0, SEEK_SET);

    shared_file = (char *)mmap(0, file_size, PROT_READ, MAP_SHARED, fd_file, 0);
    if (shared_file == (void *)-1)
    {
        message = 0;
    }

write_message:
    write_response_string("MAP_FILE!");

    if (message == 1)
    {
        write_response_string("SUCCESS!");
    }
    else
    {
        write_response_string("ERROR!");
    }
}

void do_read_from_file_offset(unsigned int offset, unsigned int no_bytes)
{
    int message = 1;

    if (fd_shm == -1 || shared_memo == (void *)-1)
    {
        printf("fd shm doesnt work\n");
        message = 0;
        goto write_message;
    }

    if (fd_file == -1 || shared_file == (void *)-1)
    {
        printf("fd file doesnt work\n");

        message = 0;
        goto write_message;
    }

    if (wrong_offset(offset, no_bytes, file_size))
    {
        printf("wrong doesnt work\n");

        message = 0;
        goto write_message;
    }

    char *aux_write = (char *)malloc(sizeof(char) * (no_bytes + 1));

    for (int i = 0; i < no_bytes; i++)
    {
        aux_write[i] = shared_file[i + offset];
    }
    aux_write[no_bytes] = 0;


    for (int i = 0; i < no_bytes; i++)
    {
        shared_memo[i] = aux_write[i];
    }


write_message:
    write_response_string("READ_FROM_FILE_OFFSET!");

    if (message == 1)
    {
        write_response_string("SUCCESS!");
    }
    else
    {
        write_response_string("ERROR!");
    }
}

void do_read_from_file_section(unsigned int section, unsigned int offset, unsigned int no_bytes)
{
    int message = 1;

    if (fd_shm == -1 || shared_memo == (void *)-1)
    {
        printf("fd shm doesnt work\n");
        message = 0;
        goto write_message;
    }

    if (fd_file == -1 || shared_file == (void *)-1)
    {
        printf("fd file doesnt work\n");

        message = 0;
        goto write_message;
    }

    if (wrong_offset(offset, no_bytes, file_size))
    {
        printf("wrong offset doesnt work\n");

        message = 0;
        goto write_message;
    }

    char nb_sections = shared_file[8];
    if (section > nb_sections)
    {
        message = 0;
        goto write_message;
    }

    int sect_aux = 9 + 25 * (section - 1) + 17;
    unsigned int off_sect = *((unsigned int *)(shared_file + sect_aux));
    char *aux_write = (char *)malloc(sizeof(char) * (no_bytes + 1));
    off_t start = offset + off_sect;

    for (int i = start; i < start + no_bytes; i++)
    {
        aux_write[i - start] = shared_file[i];
    }
    aux_write[no_bytes] = 0;
    for (int i = 0; i < no_bytes; i++)
    {
        shared_memo[i] = aux_write[i];
    }

write_message:
    write_response_string("READ_FROM_FILE_SECTION!");
    if (message == 1)
    {
        write_response_string("SUCCESS!");
    }
    else
    {
        write_response_string("ERROR!");
    }
}

void do_read_from_logical_space_offset(unsigned int logical_off, unsigned int no_bytes)
{
    // todo:
    int message = 1;

    if (fd_shm == -1 || shared_memo == (void *)-1)
    {
        printf("fd shm doesnt work\n");
        message = 0;
        goto write_message;
    }

    if (fd_file == -1 || shared_file == (void *)-1)
    {
        printf("fd file doesnt work\n");

        message = 0;
        goto write_message;
    }

    // 2(magic) + 2(hs) + 4(version) + 1(nos) = 9 ;  10 --> sectiunile

    off_t crt_offset = 9;
    char nb_sections = shared_file[8];
    int current_logic_off = 0;
    int start_offset_section = 0;
    int first_here = 1;
    int count_bytes = 0;


    crt_offset = 9;

    for (int i = 0; i < nb_sections; i++)
    {


        // determin offset-ul fisierului din header
        unsigned int offset = *((unsigned int *)(shared_file + crt_offset + 17));
        unsigned int size = *((unsigned int *)(shared_file + crt_offset + 21));

        // calculez unde are aliniamentul fiecare sectiune
        int aux = size / 2048;
        if (size % 2048 != 0)
        {
            // in cazul in care sectiunea se intinde pe mai multe zone --> adica am aliniament mai mare
            aux++;
        }

        if (current_logic_off + aux * 2048 >= logical_off)
        {
            // am ajung in zona de unde incep sa citesce

            // dimensiunea fisierului e insuficienta
            if (offset + no_bytes > file_size)
            {
                message = 0;
                goto write_message;
            }

            char *aux_write = (char *)malloc(sizeof(char) * (no_bytes + 1));

            // prima daca cand cand incep sa citesc ma asigur ca incep de la offset-ul cerut
            // dupa daca ma intind pe mai multe sectiuni citesc in continuarea sectiunilor (chiar de la inceputul fiecareia) pana ce am citi exact no_bytes
            if (first_here == 0)
            {
                start_offset_section = offset;
            }
            else
            {
                // poate nu incepe sa citeasca chiar de la inceputul sectiunii
                start_offset_section = offset + logical_off - current_logic_off;
                first_here = 0;
            }

            int last_offset_section = offset + size;


            // cat din sectiunea curenta copiez? pana la finalul ei / pana cand am numarat no_bytes
            // adica size-offset octeti

            while (count_bytes < no_bytes && start_offset_section < last_offset_section)
            {
                aux_write[count_bytes++] = shared_file[start_offset_section++];
            }

            if (count_bytes == no_bytes)
            {
                aux_write[count_bytes] = 0;

                for (int j = 0; j < no_bytes; j++)
                {
                    shared_memo[j] = aux_write[j];
                }
                free(aux_write);
                goto write_message;
            }
        }

        current_logic_off += aux * 2048;
        crt_offset += 25;
    }

write_message:
    write_response_string("READ_FROM_LOGICAL_SPACE_OFFSET!");
    if (message == 1)
    {
        write_response_string("SUCCESS!");
    }
    else
    {
        write_response_string("ERROR!");
    }
}

void do_requests_responses()
{

    for (;;)
    {
        char *s = read_request_string();

        if (strcmp(s, "PING") == 0)
        {
            do_ping(s);
        }
        else if (strcmp(s, "CREATE_SHM") == 0)
        {
            int size = read_request_number();
            do_create_shm(size);
        }
        else if (strcmp(s, "WRITE_TO_SHM") == 0)
        {
            unsigned int offset = read_request_number();
            unsigned int value = read_request_number();
            do_write_to_shm(offset, value);
        }
        else if (strcmp(s, "MAP_FILE") == 0)
        {
            char *s = read_request_string();
            do_map_file(s);
        }
        else if (strcmp(s, "READ_FROM_FILE_OFFSET") == 0)
        {
            unsigned int offset = read_request_number();
            unsigned int no_bytes = read_request_number();
            do_read_from_file_offset(offset, no_bytes);
        }
        else if (strcmp(s, "READ_FROM_FILE_SECTION") == 0)
        {
            unsigned int section = read_request_number();
            unsigned int offset = read_request_number();
            unsigned int no_bytes = read_request_number();
            do_read_from_file_section(section, offset, no_bytes);
        }
        else if (strcmp(s, "READ_FROM_LOGICAL_SPACE_OFFSET") == 0)
        {
            unsigned int logical_off = read_request_number();
            unsigned int no_bytes = read_request_number();
            do_read_from_logical_space_offset(logical_off, no_bytes);
        }

        else if (strcmp(s, "EXIT") == 0)
        {
            close(fd_req);
            close(fd_resp);
            unlink(FIFO_NAME_RESP);
            break;
        }
    }
}

int main()
{

    int x = 1;

    unlink(FIFO_NAME_RESP);

    /*   if (x == 1)
          unlink(FIFO_NAME_REQ); */

    if (x == 0)
    {
        // to be deleted
        unlink(fifo_test);
        /// to be deleted
        if (mkfifo(fifo_test, 0600) != 0)
        {
            printf("aici!1");
            printf("ERROR\ncannot create trequest pipe qith mkfifo\n");
            return 1;
        }
        ///
    }

    if (mkfifo(FIFO_NAME_RESP, 0600) != 0)
    {
        printf("ERROR\ncannot create trequest pipe qith mkfifo\n");
        return 1;
    }

    if (x == 1)
    {
        fd_req = open(FIFO_NAME_REQ, O_RDONLY);
    }

    if (x == 0)
    {
        // to be deleted
        fd_req = open(fifo_test, O_RDONLY);
    }

    if (fd_req == -1)
    {

        printf("ERROR\ncannot open the request pipe\n");
        return 1;
    }

    fd_resp = open(FIFO_NAME_RESP, O_WRONLY);

    if (fd_resp == -1)
    {
        printf("aici!3");

        printf("ERROR\ncannot create the response pipe\n");
        return 1;
    }

    write_response_string("BEGIN!");
    printf("SUCCESS\n");

    do_requests_responses();

    return 0;
}