
#include "child.h"


int main(int argc, char *argv[])
{
    int NodeNum = 0;

    int pipe_child_main[2];   // pipefd[0] refers to the read end of the pipe.  pipefd[1] refers to the write end.
    int pipe_main_child[2];   // pipefd[0] refers to the read end of the pipe.  pipefd[1] refers to the write end.

	if (argc != 3){
		printf("Usage: ./proxy <path> <number>\n");
		return -1;
	}
	if (sscanf(argv[2], "%u", &NodeNum) == 0){
		printf("Usage: ./proxy <path> <number>\n");
		return -1;
	}

	LocalNode** local_nodes = new LocalNode* [NodeNum];

    pipe_main_child[0] = open(argv[1], O_RDONLY);
    if(!pipe_main_child[0]) {perror("Open ERROR"); fflush(stderr);}

    for (int i = 0; i < NodeNum; i++)
    {
        pipe(pipe_child_main);
        if (fork() == 0)
        {
            for (int j = 0; j < i; j++) local_nodes[j]->clean();
            close(pipe_main_child[1]);
            close(pipe_child_main[0]);
            CommNode child (i, pipe_main_child[0], pipe_child_main[1], 4096);
            exit(0);
        }
        close(pipe_main_child[0]);

        pipe(pipe_main_child);
        int local_size = (3072*(NodeNum - i + 1) < MAXBUF) ? 3072*(NodeNum - i + 1) : MAXBUF;
        local_nodes [i] = new LocalNode (i, pipe_child_main[0], pipe_main_child[1], local_size);
        close(pipe_child_main[1]);
    }
    local_nodes [NodeNum - 1]->fd_out = 1;

    //
    //
    //

	fd_set readset, writeset, readset_tmp, writeset_tmp;
    FD_ZERO(&readset);
	FD_ZERO(&writeset);

    int MAX_FD = 0;
    for (int i = 0; i < NodeNum; i++)
    {
        if (local_nodes [i]->fd_in  > MAX_FD) MAX_FD = local_nodes [i]->fd_in;
        if (local_nodes [i]->fd_out > MAX_FD) MAX_FD = local_nodes [i]->fd_out;
        FD_SET(local_nodes [i]->fd_in, &readset);
    }
    MAX_FD ++;

    cerr << "Starting transmission\n";
    fflush(stderr);

    //while(  FD_ISSET(local_nodes [NodeNum-1]->fd_in, &readset) || FD_ISSET(local_nodes [NodeNum-1]->fd_out, &writeset)  )
    while(1)
    {
		readset_tmp  = readset;
		writeset_tmp = writeset;
		select(MAX_FD, &readset_tmp, &writeset_tmp, NULL, NULL);

		for (int i= 0; i < NodeNum; i++)
		{
			if (FD_ISSET(local_nodes [i]->fd_in, &readset_tmp))
			{
				int bytes_read = local_nodes [i]->receive();
				if (bytes_read <= 0)                FD_CLR(local_nodes [i]->fd_in,  &readset);
				if (local_nodes [i]->is_full  )     FD_CLR(local_nodes [i]->fd_in,  &readset );
				if (local_nodes [i]->is_finished)   continue;
				if (!local_nodes [i]->is_empty)     FD_SET(local_nodes [i]->fd_out, &writeset);
			}


			if (FD_ISSET(local_nodes [i]->fd_out, &writeset_tmp))
			{
				int bytes_written = local_nodes [i]->send();
				if (bytes_written <= 0)             FD_CLR(local_nodes [i]->fd_out, &writeset);
				if (local_nodes [i]->is_empty)      FD_CLR(local_nodes [i]->fd_out, &writeset);
                if (local_nodes [i]->in_closed)     continue;
                if (!local_nodes [i]->is_full)      FD_SET(local_nodes [i]->fd_in,  &readset );

			}
		}

        if (local_nodes [NodeNum-1]->is_finished) break;
    }

	for (int i = 0; i < NodeNum; i++) wait(NULL);
    return 0;
}

