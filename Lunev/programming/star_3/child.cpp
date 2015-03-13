
#include "child.h"



CommNode::CommNode (int id_, int f_in, int f_out, uint64 size)
{
    id = id_;
    fd_in    = f_in;
    fd_out   = f_out;
    buf_size = size;
    buf = new char [buf_size];

    while (1)
    {
        int num_of_bytes = get ();
        put (num_of_bytes);
    }
};


CommNode::~CommNode ()
{
    delete [] buf;
    close(fd_in);
    close(fd_out);
};


int CommNode::get ()
{
    int num_of_bytes = read(fd_in, buf, buf_size);
    if (num_of_bytes <=  0)
    {
        close(fd_in);
        close(fd_out);
        exit(0);
    }
    return num_of_bytes;
};


void CommNode::put (int num_of_bytes)
{
    int bytes_remain = num_of_bytes;
    while (bytes_remain > 0)
    {
        int written = write(fd_out, buf, num_of_bytes);
        for (uint64 i = 0; i < buf_size - written; i++)
            buf[i]= buf[i + written];
        bytes_remain -= written;
    }
};


int LocalNode::receive ()
{
    int bytes_read = read(fd_in, buf + filled, buf_size - filled);
    //cerr << id << " Received " << bytes_read << "bytes\n";
    //fflush(stderr);

    if (bytes_read > 0) filled += bytes_read;
    if (bytes_read <= 0)  in_closed =       true;
    is_full  = (filled == buf_size)       ? true : false;
    is_empty = (filled == 0)              ? true : false;
    is_finished = (in_closed && is_empty) ? true : false;
    if(in_closed) close(fd_in);
    if(is_finished) if(fd_out != 1) close(fd_out);
    return bytes_read;
}


int LocalNode::send ()
{
	int written = write(fd_out, buf, filled);
    //cerr << id << " Written " << written << "bytes\n";
    //fflush(stderr);
    if(written >0)
    {
        for (uint64 i = 0; i < buf_size - written; i++)
            buf[i]= buf[i + written];
        filled -= written;
    }

    is_full  = (filled == buf_size)       ? true : false;
    is_empty = (filled == 0)              ? true : false;
    is_finished = (in_closed && is_empty) ? true : false;

    if(is_finished) if(fd_out != 1) close(fd_out);
    return written;
}
