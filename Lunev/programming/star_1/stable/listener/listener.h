#ifndef LISTENER_H
#define LISTENER_H

#include "../config.h"




class Listener
{
public:
    char comm_fifo_name[PATH_MAX];
    char sync_fifo_tl_name[PATH_MAX];
    char sync_fifo_lt_name[PATH_MAX];
    char gret_fifo_name[PATH_MAX];
    int comm_fifo_desc;
    int sync_fifo_tl_desc;
    int sync_fifo_lt_desc;
    int gret_fifo_desc;
    int lpid;
private:
    unsigned char buf[BUFSIZE + 1];
    time_t start, end;
    int t_left;

public:
    Listener()
    {
        lpid = getpid();
        sprintf(comm_fifo_name, COMM_FIFO_LOC, lpid);
        sprintf(sync_fifo_tl_name, SYNC_FIFO_TL_LOC, lpid);
        sprintf(sync_fifo_lt_name, SYNC_FIFO_LT_LOC, lpid);
        sprintf(gret_fifo_name, GREETPIPE_NAME     );

        std::cerr << pathconf(comm_fifo_name, _PC_PIPE_BUF) << " bytes can be written at once\n";
        std::cerr << sysconf(_SC_OPEN_MAX) << " FIFO's can be opened at once\n";

        memset(buf, '\0', BUFSIZE+1);
    }


    ~Listener();

    int begin_transmission();
    int set_connection();

private:
    int init_fifo (const char *name, int flags);
    int output(unsigned char buf[BUFSIZE], int size);

    void err_exit()
    {
        printf(KRED);
        perror("\nError: ");
        printf(KNRM);
        exit(EXIT_FAILURE);
    }
};





#endif
