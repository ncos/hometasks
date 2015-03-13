#ifndef TALKER_H
#define TALKER_H

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>

#define MAXBUF 131072


typedef unsigned long long int uint64;

using std::cin;
using std::cerr;
using std::cout;
using std::endl;


class LocalNode
{
public:
    int id;
    int fd_in;
    int fd_out;
    char *buf;
    uint64 buf_size;
    uint64 filled;
    bool is_full;
    bool is_empty;
    bool in_closed;
    bool is_finished;

    LocalNode (int id_, int f_in, int f_out, uint64 size)
    {
        id = id_;
        fd_in = f_in;
        fd_out = f_out;
        buf_size = size;
        buf = new char [buf_size];
        is_full = false;
        is_empty = true;
        in_closed = false;
        is_finished = false;
        filled = 0;

        //fprintf(stderr, "%d: LN Constructor\n", id);
        //fflush(stderr);

        fcntl(fd_in, F_SETFL, O_NONBLOCK);
        fcntl(fd_out, F_SETFL, O_NONBLOCK);
    }

    ~LocalNode ()
    {
        //fprintf(stderr, "LN Destructor. id: %d; pid: %d\n", id, getpid());
        //fflush(stderr);
        delete [] buf;
        close(fd_in);
        close(fd_out);
    }

    void clean ()
    {
        close(fd_in);
        close(fd_out);
    }

    int receive ();
    int send ();

};



class CommNode
{
    int id;
    int fd_in;
    int fd_out;
    char *buf;
    uint64 buf_size;

private:
    int get ();
    void put (int num_of_bytes);

public:
    CommNode (int id_, int f_in, int f_out, uint64 size);
    virtual ~CommNode ();

    int getid () { return id; }

};









#endif
