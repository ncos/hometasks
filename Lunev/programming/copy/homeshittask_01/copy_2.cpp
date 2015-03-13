
#include "copy.h"




int main(int argc, char *argv[]) {
    /* checks args */
    if(argc != 3) {
        fprintf(stderr, "usage: %s source destination\n", *argv);
        exit(-1);
    }

    char *src_path = argv[1];
    char *dst_path = argv[2];



    // BUFSIZE defaults to 8192
    // BUFSIZE of 1 means one chareter at time
    // good values should fit to blocksize, like 1024 or 4096
    // higher values reduce number of system calls
    // size_t BUFFER_SIZE = 4096;

    char buf[BUFSIZ];

    int source_desc = open (src_path, O_RDONLY);
    if (source_desc < 0) {
        fprintf(stderr, "cannot open %s for reading\n", src_path);
        return -1;
    }


    struct stat src_open_sb;
    if (fstat (source_desc, &src_open_sb)) {
        fprintf(stderr, "cannot fstat %s\n", src_path);
        close_src_desc (source_desc);
        return -1;
    }

    if (( src_open_sb.st_mode & S_IFMT) != S_IFREG) {
        fprintf(stderr, "\n%s is not a regular file!\n", src_path);
        print_stats(src_open_sb, src_path);
        close_src_desc (source_desc);
        return -1;
    }

// ------------------------------------------------------------------------------------
    int dest_desc = open (dst_path, O_WRONLY | O_TRUNC | O_CREAT,
                                    S_IRUSR  | S_IWUSR | S_IRGRP | S_IROTH);
    if (dest_desc < 0) {
        if (unlink (dst_path)) {
	        fprintf(stderr, "cannot remove %s\n", dst_path);
            close_src_desc (source_desc);
            return -1;
	    }
        /* Try the open again, but this time with different flags.  */
        dest_desc = open (dst_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	}

    if (dest_desc < 0) {
        fprintf(stderr, "cannot create regular file %s\n", dst_path);
        close_src_and_dst_desc (source_desc, dest_desc);
        return -1;
    }




    struct stat dst_open_sb;
    if (fstat (dest_desc, &dst_open_sb)) {
        fprintf(stderr, "cannot fstat %s\n", dst_path);
        close_src_and_dst_desc (source_desc, dest_desc);
        return -1;
    }

    if (( dst_open_sb.st_mode & S_IFMT) != S_IFREG) {
        fprintf(stderr, "\n%s is not a regular file!\n", dst_path);
        print_stats(dst_open_sb, dst_path);
        close_src_and_dst_desc (source_desc, dest_desc);
        return -1;
    }



    //int buf_size = 10;
    //buf_size = ST_BLKSIZE (sb);



    /* Make a buffer with space for a sentinel at the end.  */
    //char *buf;
    //buf = (char *) alloca (buf_size + sizeof (int));



    size_t size;
    while ((size = read(source_desc, buf, BUFSIZ)) > 0) {
        write(dest_desc, buf, size);
    }
    return close_src_and_dst_desc (source_desc, dest_desc);
}






void print_stats(struct stat sb, const char *path) {
    printf("File path:                %s\n", path);
    printf("File type:                ");

    switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:  printf("block device\n");            break;
        case S_IFCHR:  printf("character device\n");        break;
        case S_IFDIR:  printf("directory\n");               break;
        case S_IFIFO:  printf("FIFO/pipe\n");               break;
        case S_IFLNK:  printf("symlink\n");                 break;
        case S_IFREG:  printf("regular file\n");            break;
        case S_IFSOCK: printf("socket\n");                  break;
        default:       printf("unknown?\n");                break;
        }

    printf("I-node number:            %ld\n", (long) sb.st_ino);
    printf("Mode:                     %lo (octal)\n",   (unsigned long) sb.st_mode);

    printf("Link count:               %ld\n", (long) sb.st_nlink);
    printf("Ownership:                UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);

    printf("Preferred I/O block size: %ld bytes\n",     (long) sb.st_blksize);
    printf("File size:                %lld bytes\n",    (long long) sb.st_size);
    printf("Blocks allocated:         %lld\n",          (long long) sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));
}


int close_src_and_dst_desc (int source_desc, int dest_desc) {
    if (close (dest_desc) < 0) {
        fprintf(stderr, "closing destination file failure\n");
        return -1;
    }
    return close_src_desc (source_desc);
}


int close_src_desc (int source_desc) {
    if (close (source_desc) < 0) {
        fprintf(stderr, "closing source file failure\n");
        return -1;
    }
    return 1;
}
