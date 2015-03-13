
#include "talker.h"


int Talker::init_fifo(const char *name, int flags)
{
    if ( mkfifo(name, 0777) && (errno != EEXIST ) )
    {
        printf(KRED);
        printf("[%d]: ", getpid());
        perror("mkfifo");
        printf(KNRM);
        exit(EXIT_FAILURE);
    }

    int fd = 0;
    if ( (fd = open(name, flags)) <= 0 )
    {
        printf(KRED);
        printf("[%d]: ", getpid());
        perror("open");
        printf(KNRM);
        exit(EXIT_FAILURE);
    }

    return fd;
}


Talker::~Talker()
{
    close (comm_fifo_desc);
    close (sync_fifo_tl_desc);
    close (sync_fifo_lt_desc);
    close (source_desc);
}


int Talker::set_connection()
{
    sprintf(gret_fifo_name, GREETPIPE_NAME);
    gret_fifo_desc = init_fifo (gret_fifo_name, O_NONBLOCK | O_RDONLY);

    time(&start);
    __WARN("Awaiting a free listener at \""<< gret_fifo_name << "\"... \n");
    while ( read(gret_fifo_desc, &lpid, sizeof(int)) != sizeof(int) )
    {
        time(&end);
        t_left = TIMEOUT-difftime (end, start);
        printf("\b\b\b\b\b\b%d (s)", t_left);
        if(t_left <= 0) err_exit();
    }
    __CHECK(close(gret_fifo_desc));
    sprintf(comm_fifo_name, COMM_FIFO_LOC, lpid);
    sprintf(sync_fifo_tl_name, SYNC_FIFO_TL_LOC, lpid);
    sprintf(sync_fifo_lt_name, SYNC_FIFO_LT_LOC, lpid);

    __CHECK(comm_fifo_desc = open (comm_fifo_name,  O_NONBLOCK | O_WRONLY) );
    __CHECK(sync_fifo_tl_desc = open (sync_fifo_tl_name,  O_NONBLOCK | O_RDWR  ) );
    __CHECK(sync_fifo_lt_desc = open (sync_fifo_lt_name,  O_NONBLOCK | O_RDWR  ) );


    // paths are configured and fifos are ready at this point ----------------------------------

    int code = READY_SQ;
    int check_len = write(sync_fifo_tl_desc, &code, sizeof(int));
    if(check_len != sizeof(int)) err_exit();


    // the listener is informed that we are ready -----------------------------------------------

    __NTFY("The READY signal sent. Connection established\n");
    return EXIT_SUCCESS;
}

/*
int Talker::begin_transmission(int fd)
{
    if (comm_fifo_desc <= 0) throw std::runtime_error("The communication FIFO suddenly closed. WTF?!\n"  );
    if (sync_fifo_tl_desc <= 0) throw std::runtime_error("The output synchronization FIFO suddenly closed. WTF?!\n");
    if (sync_fifo_lt_desc <= 0) throw std::runtime_error("The input synchronization FIFO suddenly closed. WTF?!\n" );

    memset(buf, '\0', BUFSIZE+1);


    int size, code, sent, i = 0;
    while ((size = read(fd, buf, BUFSIZE)) > 0)
    {
        i++;
        //printf("%s\n", buf);
        printf ("[pack %d]: writing...   ---   ", i);
       __CHECK(sent = write(comm_fifo_desc, buf, size) );
        printf ("[pack %d]: Written %d   ---   \n", i, sent);
        memset(buf, '\0', BUFSIZE);


        // waiting until we get the READ_SS code in the sync fifo

        time(&start);
        while ( read(sync_fifo_lt_desc, &code, sizeof(int)) != sizeof(int) )
        {
            time(&end);
            t_left = TIMEOUT-difftime (end, start);
            printf("\b\b\b\b\b\b%d (s)", t_left);
            if(t_left <= 0) err_exit();
        }

        printf ("\n-------------------------------[pack %d]: reader received   ---   \n", i);


        if (code != READ_SS) throw std::runtime_error("Wrong start code received (not READ_SS). Contact your system administrator)\n");

    }

    code = STOP_SQ;
    int check_len = write(sync_fifo_tl_desc, &code, sizeof(int));
    if(check_len != sizeof(int)) err_exit();

    return EXIT_SUCCESS;
}
*/


int Talker::begin_transmission(int fd)
{
    if (comm_fifo_desc <= 0) throw std::runtime_error("The communication FIFO suddenly closed. WTF?!\n"  );
    if (sync_fifo_tl_desc <= 0) throw std::runtime_error("The output synchronization FIFO suddenly closed. WTF?!\n");
    if (sync_fifo_lt_desc <= 0) throw std::runtime_error("The input synchronization FIFO suddenly closed. WTF?!\n" );

    memset(buf, '\0', BUFSIZE+1);


    printf("start\n");

    int fd = open(comm_fifo_name, O_RDWR);
    write(fd, "aaa", 4);

    fd = open(comm_fifo_name, O_RDONLY|O_NONBLOCK);
    read(fd, buf, 4);
    write(1, buf, 4);
    perror("test ");


    printf("stop\n");

    int flags = fcntl(comm_fifo_desc, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(comm_fifo_desc, F_SETFL, flags);


    int size, code, sent, i = 0;
    while ((size = read(fd, buf, BUFSIZE)) > 0)
    {
        i++;
        //printf("%s\n", buf);
        printf ("[pack %d]: writing...   ---   ", i);
       __CHECK(sent = write(comm_fifo_desc, buf, size) );
        printf ("[pack %d]: Written %d   ---   \n", i, sent);
        memset(buf, '\0', BUFSIZE);
    }



    return EXIT_SUCCESS;
}





int Talker::input()
{/*
    printf(KCYN);
    printf("[%d]: ", getpid());
    scanf("%s", buf);
    strcat(buf, "\n");
    printf(KNRM);
    printf("\n");  */

    size_t size;
    if ((size = read(source_desc, buf, BUFSIZE)) <= 0) return EXIT_FAILURE;
    printf("%s", buf);

    return EXIT_SUCCESS;
}


