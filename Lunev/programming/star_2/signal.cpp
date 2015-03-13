#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>


//Global





int main (int argc, char** argv)
{
    parentpid = getpid();
    childpid = fork();


    if (childpid == -1)
    {
        perror ("fork");
        exit (EXIT_FAILURE);
    }


	if (childpid == 0)
        child_proc(argv);



    int 	n    = 0;
    char	data = 0;

    connection = 0;

    sigset_t sigset_usr1, sigset_usr2, sigset_child;

    sigemptyset (&sigset_usr1);
    sigemptyset (&sigset_usr2);
    sigemptyset (&sigset_child);

    sigaddset (&sigset_usr1,  SIGUSR1);
	sigaddset (&sigset_usr2,  SIGUSR2);
	sigaddset (&sigset_child, SIGCHLD);

	struct sigaction sigact_usr1, sigact_usr2, sigact_child;

	memset (&sigact_usr1,  0, sizeof(sigact_usr1));
	memset (&sigact_usr2,  0, sizeof(sigact_usr2));
	memset (&sigact_child, 0, sizeof(sigact_child));

	sigact_usr1  . sa_handler  = get_signal_0;
	sigact_usr2  . sa_handler  = get_signal_1;
	sigact_child . sa_handler  = get_signal_die;

	sigaction (SIGUSR1, &sigact_usr1,  NULL);
	sigaction (SIGUSR2, &sigact_usr2,  NULL);
	sigaction (SIGCHLD, &sigact_child, NULL);

	sigprocmask (SIG_UNBLOCK, &sigset_usr1,  NULL);
	sigprocmask (SIG_UNBLOCK, &sigset_usr2,  NULL);
	sigprocmask (SIG_UNBLOCK, &sigset_child, NULL);

	sleep(1);

	kill(childpid, SIGPIPE);



	while (1)
	{
		sigprocmask (SIG_UNBLOCK, &sigset_usr1, NULL);
		sigprocmask (SIG_UNBLOCK, &sigset_usr2, NULL);

		while (!connection);
        connection = 0;

		sigprocmask(SIG_BLOCK, &sigset_usr1, NULL);
		sigprocmask(SIG_BLOCK, &sigset_usr2, NULL);

		data = data + (bit << n);

		if (n < 7)
			n++;
		else
		{
			printf ("%c", data);
			fflush (stdout);

			data = 0;
			n = 0;
		}

		kill(childpid, SIGPIPE);
    }

	wait(NULL);

return 0;
}



