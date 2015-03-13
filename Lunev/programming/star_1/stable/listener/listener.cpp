
#include "listener.h"

int Listener::init_fifo(const char *name, int flags)
{
    if ( mkfifo(name, 0777) && (errno != EEXIST ) )
    {
        std::cerr << KRED;
        perror("mkfifo");
        std::cerr << KNRM;
        exit(EXIT_FAILURE);
    }

    int fd = 0;
    if ( (fd = open(name, flags)) <= 0 )
    {
        std::cerr << KRED;
        perror("open");
        std::cerr << KNRM;
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
    __WARN("Awaiting greeting fifo to open on the other side \"" << GREETPIPE_NAME << "\"... \n");
    while( (gret_fifo_desc = open (GREETPIPE_NAME,  O_NONBLOCK | O_WRONLY)) <= 0)
    {
        time(&end);
        t_left = TIMEOUT-difftime (end, start);
        std::cerr << "\b\b\b\b\b\b" << t_left << "(s)";
        if(t_left <= 0) err_exit();
    }
    std::cerr << std::endl;


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
        std::cerr << "\b\b\b\b\b\b" << t_left << "(s)";
    }
    std::cerr << std::endl;

    if (code != READY_SQ) throw std::runtime_error("Wrong start code received. Contact your system administrator)\n");

    // talker have sent a sygnal that he is ready ----------------------------------------------

    __NTFY("The READY signal received. Connection established. Closing the greeting pipe...\n");

    //__CHECK(close(gret_fifo_desc));
    return EXIT_SUCCESS;
}

/*
int Listener::begin_transmission()
{
    if (comm_fifo_desc <= 0) throw std::runtime_error("The communication FIFO suddenly closed. WTF?!\n"  );
    if (sync_fifo_tl_desc <= 0) throw std::runtime_error("The input synchronization FIFO suddenly closed. WTF?!\n" );
    if (sync_fifo_lt_desc <= 0) throw std::runtime_error("The output synchronization FIFO suddenly closed. WTF?!\n");


    int flags = fcntl(comm_fifo_desc, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(comm_fifo_desc, F_SETFL, flags);


    int size = -1, code = -1;
    bool wdt = false;
    while (1)
    {
        size = read(sync_fifo_tl_desc, &code, sizeof(int));


        if( (size == sizeof(int))&&(code == STOP_SQ) )
        {
            __NTFY("Talker have sent the STOP signal. Apparently all information was read\n");
            return EXIT_SUCCESS;
        }

        memset(buf, '\0', BUFSIZE);
sleep(1);
        size = read(comm_fifo_desc, buf, BUFSIZE);

        if(size <= 0)
        {
            if(!wdt)
            {
                time(&start);
                __WARN("Talker not responding, shutting down in \n");
            }

            wdt = true;
            time(&end);
            t_left = TIMEOUT-difftime (end, start);
            if(t_left <= 0) err_exit();
            std::cerr << "\r" << t_left << "(s)";
        }
        else
        {
            if(wdt) std::cerr << std::endl;
            wdt = false;
            output(buf, size);

            code = READ_SS;
            int check_len = write(sync_fifo_lt_desc, &code, sizeof(int));
            if(check_len != sizeof(int)) err_exit();
        }

    }

    return EXIT_FAILURE;
}
*/



int Listener::begin_transmission()
{
    if (comm_fifo_desc <= 0) throw std::runtime_error("The communication FIFO suddenly closed. WTF?!\n"  );
    if (sync_fifo_tl_desc <= 0) throw std::runtime_error("The input synchronization FIFO suddenly closed. WTF?!\n" );
    if (sync_fifo_lt_desc <= 0) throw std::runtime_error("The output synchronization FIFO suddenly closed. WTF?!\n");


    int flags = fcntl(comm_fifo_desc, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(comm_fifo_desc, F_SETFL, flags);


    memset(buf, '\0', BUFSIZE);
    int size = read(comm_fifo_desc, buf, BUFSIZE);
    output(buf, size);


    while (size > 0)
    {
        memset(buf, '\0', BUFSIZE);
        size = read(comm_fifo_desc, buf, BUFSIZE);
        sleep(1);
        output(buf, size);
    }
    return EXIT_FAILURE;
}




int Listener::output(unsigned char buf[BUFSIZE], int size)
{
    write(1, buf, size);

    return EXIT_SUCCESS;
}
