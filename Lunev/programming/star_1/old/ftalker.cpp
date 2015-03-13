
#include "communication.h"


int get_lpid()
{
    int fd = init_fifo (GREETPIPE_NAME, O_RDONLY);


    int lpid = -1;
    if (read(fd, &lpid, sizeof(int)) <= 0 )
    {
        printf(KRED);
        printf("[%d]: No free listener for me? Shutting down :(\n", getpid());
        printf(KNRM);
        exit(0);
    }
    close(fd);


    return lpid;
}


int set_connection(int lpid)
{
    char fifo_name[PATH_MAX];
    get_fifo_name(lpid, fifo_name);


    int fd_connection = 0;
    if ( (fd_connection = open(fifo_name, O_NONBLOCK | O_WRONLY)) <= 0 )
    {
        printf(KRED);
        perror("open");
        printf(KNRM);
        exit(EXIT_FAILURE);
    }


    printf(KYEL);
    printf("\n[%d]: Subscribed to a fifo \"%s\"\n\n", getpid(), fifo_name);
    printf(KNRM);


    printf(KBLU);
    printf("[%d]: Connection established\n", getpid());
    printf(KNRM);

    return fd_connection;
}





int main(int argc, char *argv[])

{
    printf(KYEL);
    printf("Invoked talker [%d]\n", getpid());
    printf(KNRM);

    //printf("Можно записать в FIFO сразу %ld байтов\n", pathconf(GREETPIPE_NAME, _PC_PIPE_BUF)) ;
    //printf("Одновременно можно открыть %ld FIFO \n", sysconf(_SC_OPEN_MAX));

    int lpid = get_lpid();
    int fd = set_connection(lpid);


    char buf[BUFSIZE];
    memset(buf, '\0', BUFSIZE);
    while(1)
    {
        //char output[BUFSIZE];

        printf(KCYN);
        printf("[%d]: ", getpid());
        scanf("%s", buf);
        strcat(buf, "\n");
        printf(KNRM);
        write(fd, buf, BUFSIZE);
    }

    close(fd);

    return 0;
}

