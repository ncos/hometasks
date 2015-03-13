
#include "star_2.h"

void hl_alarm(int sig)
{
    printf("alarm...\n");
    exit(0);
}


void hl_sync(int sig)
{
    //printf("sync...\n");
    connection = 1;
}


char get_next_bit()
{
    char next_bit = (byte >> (7-i))&1;
    i ++;
    if (i > 7) i = 0;
    return next_bit;
}


int check_connection()
{
    connection = 0;
    alarm(10);

    sigset_t mask;
 	sigemptyset (&mask);
    sigsuspend (&mask);

    alarm(10);
    return 1;
}



void sender(char* path)
{

    FILE *file;
    if ( !(file = fopen (path, "r")) )
    {
        perror ("fopen");
        exit(-1);
    }

// Init

    sigemptyset (&sigset_pipe);
    sigemptyset (&sigset_alarm);

    sigaddset (&sigset_pipe,  SIGPIPE);
    sigaddset (&sigset_alarm, SIGALRM);

    sigprocmask (SIG_BLOCK,   &sigset_pipe,  NULL);
    sigprocmask (SIG_UNBLOCK, &sigset_alarm, NULL);

// Actions

    struct sigaction sigact[2];

    memset (&sigact, 0, sizeof(sigact));

    sigact[0].sa_handler = hl_sync;
    sigact[1].sa_handler = hl_alarm;

    sigaction (SIGPIPE, &sigact[0], NULL);
    sigaction (SIGALRM, &sigact[1], NULL);

    if (kill (receiverpid, SIGUSR1)<0) exit(-1); // starting transmission

    i = 0;
    while (fscanf (file, "%c", &byte) > 0)
    {
        //printf("sender: byte, %c : %d\n", byte, byte);
        for (int j = 0; j < 8; j++)
        {
            check_connection();

            if (get_next_bit() == 0)
                {if (kill (receiverpid, SIGUSR1)<0) exit(-1); /*printf("sender: 0\n");*/}
            else
                {if (kill (receiverpid, SIGUSR2)<0) exit(-1); /*printf("sender: 1\n");*/}

        }
    }

    exit(0);
};
