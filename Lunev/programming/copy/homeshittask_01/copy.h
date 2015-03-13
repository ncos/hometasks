#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>   // open
#include <unistd.h>  // read, write, close
#include <cstdio>    // BUFSIZ
#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>



int close_src_and_dst_desc (int source_desc, int dest_desc);


int close_src_desc (int source_desc);



void print_stats(struct stat sb, const char *path);
