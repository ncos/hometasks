#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#include <sys/wait.h>


struct pirate_msgbuf {
    long mtype;  /* must be positive */
    int pid;
};



int main (int argc, char *argv[])
{
    //unsigned long int n = 1000;
    if(argc != 2)
    {
        fprintf(stderr, "usage: %s input number\n", *argv);
        exit(-1);
    }
    unsigned long int n = strtol(argv[1], NULL, 10);

    //----------------------------------


    int msqid = msgget (IPC_PRIVATE, 0666 | IPC_CREAT);


    struct pirate_msgbuf pmb[n+1];
    struct pirate_msgbuf pmb_r[n+1];


    for(unsigned long int i = 0; i < n+1; i++)
    {
        pmb[i].mtype = i+1;
    }




    msgsnd(msqid, &pmb[0], sizeof(struct pirate_msgbuf) - sizeof(long), 0);



    //struct pirate_msgbuf pmb_2;
    //msgrcv(msqid, &pmb_2, sizeof(struct pirate_msgbuf) - sizeof(long), 2, 0);





    //fork();
    int last_pid;
    for (unsigned long int i = 0; i < n; i++)
    {
        last_pid = fork();
        if(last_pid == 0)
        {
            msgrcv(msqid, &pmb_r[i], sizeof(struct pirate_msgbuf) - sizeof(long), i+1, 0);
            printf("%ld ", i);
            fflush(stdout);

            msgsnd(msqid, &pmb[i+1], sizeof(struct pirate_msgbuf) - sizeof(long), 0);

            return 0;
        }
        //wait(&status);
    }

    msgrcv(msqid, &pmb_r[n], sizeof(struct pirate_msgbuf) - sizeof(long), n+1, 0);

    return 0;
}
