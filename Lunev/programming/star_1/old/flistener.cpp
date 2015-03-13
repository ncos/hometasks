
#include "communication.h"


int set_connection()
{
    char fifo_name[PATH_MAX];
    get_fifo_name(getpid(), fifo_name);


    int fd_connection = init_fifo (fifo_name,  O_NONBLOCK | O_RDONLY);


    printf(KYEL);
    printf("\n[%d]: Created a fifo \"%s\", waiting for response... \n\n", getpid(), fifo_name);
    printf(KNRM);


    int lpid = getpid();
    int fd_greeting = init_fifo (GREETPIPE_NAME, O_WRONLY);
    write(fd_greeting, &lpid, sizeof(int));


    printf(KBLU);
    printf("[%d]: Connection established\n", getpid());
    printf(KNRM);


    return fd_connection;
}


int main(int argc, char *argv[])
{
    int fd = set_connection();


    char buf[BUFSIZE];
    memset(buf, '\0', BUFSIZE);


    while(1)
    {
        int size = read(fd, buf, BUFSIZE);
        if (size > 0)
        {
            printf(KCYN);
            printf("[%d] (%d bytes): %s", getpid(), size, buf);
            printf(KNRM);
        }
        else
        {
            //exit(1);
        }
    }


    close(fd);
    char fifo_name[PATH_MAX];
    get_fifo_name(getpid(), fifo_name);
    remove(fifo_name);


    remove(GREETPIPE_NAME);
    return 0;
}

