
#include "listener.h"

int Listener::init_fifo(const char *name, int flags)
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


Listener::~Listener()
{
    __CHECK(close (comm_fifo_desc));
    __CHECK(remove(comm_fifo_name));
    __CHECK(close (sync_fifo_tl_desc));
    __CHECK(close (sync_fifo_lt_desc));
    __CHECK(remove(sync_fifo_tl_name));
    __CHECK(remove(sync_fifo_lt_name));
}


int Listener::set_connection()
{
    comm_fifo_desc = init_fifo (comm_fifo_name,  O_NONBLOCK | O_RDONLY);
    sync_fifo_tl_desc = init_fifo (sync_fifo_tl_name,  O_NONBLOCK | O_RDWR  );
    sync_fifo_lt_desc = init_fifo (sync_fifo_lt_name,  O_NONBLOCK | O_RDWR  );


    time(&start);
    __WARN("Awaiting greeting fifo to open on the other side \"%s\"... \n", GREETPIPE_NAME);
    while( (gret_fifo_desc = open (GREETPIPE_NAME,  O_NONBLOCK | O_WRONLY)) <= 0)
    {
        time(&end);
        t_left = TIMEOUT-difftime (end, start);
        printf("\b\b\b\b\b\b%d (s)", t_left);
        if(t_left <= 0) err_exit();
    }

    int check_len = write(gret_fifo_desc, &lpid, sizeof(int));
    if(check_len != sizeof(int)) err_exit();


    // talker is notified that he is to talk with us at this point -----------------------------

    time(&start);
    int size, code;
    __WARN("Talker detected, awaiting start sequence... \n");
    while ((size = read(sync_fifo_tl_desc, &code, sizeof(int)) ) != sizeof(int))
    {
        time(&end);
        t_left = TIMEOUT-difftime (end, start);
        if(t_left <= 0) err_exit();
        printf("\b\b\b\b\b\b%d (s)", t_left);
    }

    printf("received %d\n", code);
    if (code != READY_SQ) throw std::runtime_error("Wrong start code received. Contact your system administrator)\n");

    // talker have sent a sygnal that he is ready ----------------------------------------------

    __NTFY("The READY signal received. Connection established. Closing the greeting pipe...\n");

    //__CHECK(close(gret_fifo_desc));
    return EXIT_SUCCESS;
}


int Listener::begin_transmission()
{
    if (comm_fifo_desc <= 0) throw std::runtime_error("The communication FIFO suddenly closed. WTF?!\n"  );
    if (sync_fifo_tl_desc <= 0) throw std::runtime_error("The input synchronization FIFO suddenly closed. WTF?!\n" );
    if (sync_fifo_lt_desc <= 0) throw std::runtime_error("The output synchronization FIFO suddenly closed. WTF?!\n");


    int size = -1, code = -1;
    bool wdt = false;
    while (1)
    {
        if( read(sync_fifo_tl_desc, &code, sizeof(int)) == sizeof(int) )
        switch (code)
        {
            case STOP_SQ:
                __NTFY("Talker have sent the STOP signal. Apparently all information was read\n");
                return EXIT_SUCCESS;
                break;

            case WRITE_SS:

        }




        memset(buf, '\0', BUFSIZE);
        size = read(comm_fifo_desc, buf, BUFSIZE);

        if(size <= 0)
        {
            if(!wdt)
            {
                time(&start);
                //__WARN("Talker not responding, shutting down in \n");
            }

            wdt = true;
            time(&end);
            t_left = TIMEOUT-difftime (end, start);
            if(t_left <= 0) err_exit();
            //printf("%d (s)      \r", t_left);
        }
        else
        {
            //if(wdt) printf("\n");
            wdt = false;
            output(buf, size);

            code = READ_SS;
            int check_len = write(sync_fifo_lt_desc, &code, sizeof(int));
            if(check_len != sizeof(int)) err_exit();
        }

    }

    return EXIT_FAILURE;
}



int Listener::output(char buf[BUFSIZE], int size)
{
    //printf(KCYN);
    printf("%s", buf);
    //printf("[%d] (%d bytes): %s", getpid(), size, buf);
    //printf(KNRM);

    return EXIT_SUCCESS;
}
