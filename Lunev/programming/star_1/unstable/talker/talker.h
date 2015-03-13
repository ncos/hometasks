#ifndef TALKER_H
#define TALKER_H

#include "../config.h"




class Talker
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
    int source_desc;
    int tpid;
    int lpid;
private:
    char buf[BUFSIZE + 1];
    time_t start, end;
    int t_left;

public:
    Talker()
    {
        tpid = getpid();
        __WARN("Invoked talker\n");
    }


    ~Talker();

    int begin_transmission(int fd);
    int set_connection();

private:
    int init_fifo (const char *name, int flags);
    int input();

    void err_exit()
    {
        printf(KRED);
        perror("\nError: ");
        printf(KNRM);
        exit(EXIT_FAILURE);
    }
};





#endif
