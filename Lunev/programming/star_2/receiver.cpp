
#include "star_2.h"


void hl_exit(int sig)
{
    //printf("exiting...\n");
    kill(senderpid, SIGKILL);
    exit(0);
}


void hl_zero(int sig)
{
    //printf("zero...\n");
    connection = 1;
    byte = (byte << 1);
    //printf ("------ %d\n", byte);
    i ++;
}


void hl_one(int sig)
{
    //printf("one...\n");
    connection = 1;
    byte = (byte << 1);
    byte ++;
    //printf ("------ %d\n", byte);
    i ++;
}


void receiver()
{
    sigemptyset (&sigset_usr1);
    sigemptyset (&sigset_usr2);
    sigemptyset (&sigset_child);

    sigaddset (&sigset_usr1,  SIGUSR1);
	sigaddset (&sigset_usr2,  SIGUSR2);
	sigaddset (&sigset_child, SIGCHLD);


    struct sigaction sigact[3];
    memset (&sigact, 0, sizeof(sigact));

    sigact[0].sa_handler = hl_zero;
    sigact[1].sa_handler = hl_one;
    sigact[2].sa_handler = hl_exit;

	sigaction (SIGUSR1, &sigact[0], NULL);
	sigaction (SIGUSR2, &sigact[1], NULL);
	sigaction (SIGCHLD, &sigact[2], NULL);

	sigprocmask (SIG_BLOCK, &sigset_usr1,  NULL);
	sigprocmask (SIG_BLOCK, &sigset_usr2,  NULL);
	sigprocmask (SIG_UNBLOCK, &sigset_child, NULL);


    sigset_t mask;
    sigemptyset (&mask); // for sigsuspend
    sigsuspend (&mask); // waiting

	kill(senderpid, SIGPIPE); // ready to receive

    byte = 0;
    i = 0;
	while (1)
	{
        sigsuspend (&mask); // waiting

		if (i > 7)
		{
			printf ("%c", byte);
			fflush (stdout);

			byte = 0;
			i = 0;
		}

		kill(senderpid, SIGPIPE);
    }
}
