#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
//http://stackoverflow.com/questions/10195343/copy-a-file-in-an-sane-safe-and-efficient-way

/* Copy a regular file from SRC_PATH to DST_PATH.
   If the source file contains holes, copies holes and blocks of zeros
   in the source file as holes in the destination file.
   (Holes are read as zeroes by the `read' system call.)
   Use DST_MODE as the 3rd argument in the call to open.
   X provides many option settings.
   Return 0 if successful, -1 if an error occurred.
   *NEW_DST is as in copy_internal.  SRC_SB is the result
   of calling xstat (aka stat in this case) on SRC_PATH. NEW_DST should be nonzero if the file DST_PATH cannot
   exist because its parent directory was just created; NEW_DST should
   be zero if DST_PATH might already exist.  */
int simple_cp(const char *src_path, const char *dst_path);
static int copy_reg (const char *src_path, const char *dst_path);
int close_src_and_dst_desc (int source_desc, int dest_desc);
int close_src_desc (int source_desc);


/*
int main(int argc, char *argv[])
{
    //checks args
    if(argc != 3)
    {
        fprintf(stderr, "usage: %s source destination\n", *argv);
        exit(-1);
    }

    char *src = argv[1];
    char *dest = argv[2];

    // received arguments from command prompt
    //simple_cp (src, dest);
    copy_reg  (src, dest);

    return 0;
}
*/


int simple_cp(const char *src_path, const char *dst_path)
{
    char command[50];

    strcpy (command, "cp "      );
    strcat (command, src_path   );
    strcat (command, " "        );
    strcat (command, dst_path   );
    return system (command      );
}



static int copy_reg (const char *src_path, const char *dst_path)
{




    //char *cp;


    //int last_write_made_hole = 0;
    //int make_holes = (x->sparse_mode == SPARSE_ALWAYS);





    int source_desc = open (src_path, O_RDONLY);
    if (source_desc < 0)
    {
        fprintf(stderr, "cannot open input file for reading\n");
        return -1;
    }


    struct stat src_open_sb;
    if (fstat (source_desc, &src_open_sb))
    {
        fprintf(stderr, "cannot fstat %s\n", src_path);
        close_src_desc (source_desc);
        return -1;
    }


  /* Compare the source dev/ino from the open file to the incoming,
     saved ones obtained via a previous call to stat.  */
//  if (! SAME_INODE (*src_sb, src_open_sb))
//    {
//     error (0, 0,
//	     _("skipping file %s, as it was replaced while being copied"),
//	     quote (src_path));
//      return_val = -1;
//    goto close_src_desc;
//    }


    int dest_desc = open (dst_path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (dest_desc < 0)
	{
        if (unlink (dst_path))
	    {
	        fprintf(stderr, "cannot remove %s\n", dst_path);
            close_src_desc (source_desc);
            return -1;
	    }
        /* Try the open again, but this time with different flags.  */
        dest_desc = open (dst_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	}


    if (dest_desc < 0)
    {
        fprintf(stderr, "cannot create regular file %s\n", dst_path);
        close_src_desc (source_desc);
        return -1;
    }


    /* Determine the optimal buffer size.  */
    struct stat sb;
    if (fstat (dest_desc, &sb))
    {
        fprintf(stderr, "cannot fstat %s\n", dst_path);
        close_src_and_dst_desc (source_desc, dest_desc);
        return -1;
    }


    int buf_size = 10;
    //buf_size = ST_BLKSIZE (sb);



    /* Make a buffer with space for a sentinel at the end.  */
    char *buf;
    buf = (char *) alloca (buf_size + sizeof (int));




    off_t n_read_total = 0;
    //int *ip;
    for (;;)
    {
        ssize_t n_read = read (source_desc, buf, buf_size);
        if (n_read < 0)
        {
            fprintf(stderr, "reading %s\n", src_path);
            close_src_and_dst_desc (source_desc, dest_desc);
            return -1;
        }

        if (n_read == 0) break;

        n_read_total += n_read;

        //ip = 0;
        /*
        if (make_holes)
        {
            buf[n_read] = 1;	// Sentinel to stop loop.

            // Find first nonzero *word*, or the word with the sentinel.

	  ip = (int *) buf;
	  while (*ip++ == 0)
	    ;

	  // Find the first nonzero *byte*, or the sentinel.

	  cp = (char *) (ip - 1);
	  while (*cp++ == 0)
	    ;

	  // If we found the sentinel, the whole input block was zero,
    //  and we can make a hole.

	  if (cp > buf + n_read)
	    {
	      // Make a hole.
	      if (lseek (dest_desc, (off_t) n_read, SEEK_CUR) < 0L)
		{
		  error (0, errno, _("cannot lseek %s"), quote (dst_path));
		  return_val = -1;
		  goto close_src_and_dst_desc;
		}
	      last_write_made_hole = 1;
	    }
	  else
	    // Clear to indicate that a normal write is needed.
	    ip = 0;
	} */
/*
        if (ip == 0)
        {
            size_t n = n_read;
            if (full_write (dest_desc, buf, n) != n)
            {
                fprintf(stderr, "writing %s\n", dst_path);
                return_val = -1;
                goto close_src_and_dst_desc;
            }
            last_write_made_hole = 0;
        } */

    } // end of the cycle



  /* If the file ends with a `hole', something needs to be written at
     the end.  Otherwise the kernel would truncate the file at the end
     of the last write operation.  */
/*
    if (last_write_made_hole)
    {
        // Write a null character and truncate it again.
        if (full_write (dest_desc, "", 1) != 1
                || ftruncate (dest_desc, n_read_total) < 0)

        {
            fprintf(stderr, "writing %s\n", dst_path);
            return_val = -1;
        }
    }

*/

    // exiting
    return close_src_and_dst_desc (source_desc, dest_desc);
}




