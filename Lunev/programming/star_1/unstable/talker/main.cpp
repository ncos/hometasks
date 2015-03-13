
#include "talker.h"


int main(int argc, char *argv[])
{
    /* checks args */
    if(argc != 2)
    {
        fprintf(stderr, "usage: %s source\n", *argv);
        exit(EXIT_FAILURE);
    }

    int source_desc = open (argv[1], O_RDONLY);
    if (source_desc < 0)
    {
            fprintf(stderr, "cannot open %s for reading\n", argv[1]);
            exit(-1);
    }


    struct stat src_open_sb;
    if (fstat (source_desc, &src_open_sb))
    {
            fprintf(stderr, "cannot fstat %s\n", argv[1]);
            exit(-1);
    }

    if (( src_open_sb.st_mode & S_IFMT) != S_IFREG)
    {
            fprintf(stderr, "\n%s is not a regular file!\n", argv[1]);
            exit(-1);
    }


    Talker talker;

    talker.set_connection();
    talker.begin_transmission(source_desc);


    return 1;
}

