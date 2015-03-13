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
    char buf[BUFSIZE + 1];
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

        memset(buf, '\0', BUFSIZE+1);
    }

    ~Listener();

    int begin_transmission();
    int set_connection();

private:
    int init_fifo (const char *name, int flags);
    int output(char buf[BUFSIZE], int size);

    void err_exit()
    {
        printf(KRED);
        perror("\nError: ");
        printf(KNRM);
        __CHECK(close (comm_fifo_desc));
        __CHECK(remove(comm_fifo_name));
        __CHECK(close (sync_fifo_tl_desc));
        __CHECK(close (sync_fifo_lt_desc));
        __CHECK(remove(sync_fifo_tl_name));
        __CHECK(remove(sync_fifo_lt_name));
        exit(EXIT_FAILURE);
    }
};





#endif
