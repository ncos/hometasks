
/**
        C++ Producer Consumer Problem by Anton Mitrokhin
*/


#include "config.h"


int removeItemFromBuffer(unsigned char *buf)
{
    for(int i = 0; i < MEMSIZE + 1; i++)
        buf[i] = (shm_package->shm_buf[i]);
    return (shm_package->size);
}



void consumer()
{
    shm_init();

    unsigned char buf [MEMSIZE + 1];
    memset(buf, '\0', MEMSIZE + 1 );

    int size = 1;
    while (size > 0)
    {
        safe_down(__fillCount__, __taklalive__);

        size = removeItemFromBuffer(buf  );

        safe_up(__emptyCount__, __taklalive__);
        write(1, buf, size     );
        memset(buf, '\0', MEMSIZE );
    }

    up(__finishFlag__);
}




int main(int argc, char *argv[])
{
    if ((key = ftok("deadspace2.exe", 127)) == -1)
    {
        perror("ftok");
        exit(-1);
    }

    /* grab the semaphore set created by seminit.c: */
    if ((semid = initsem(key, 5)) == -1)
    {
        perror("initsem");
        exit(-1);
    }


    set_alive(__listalive__);

    waitone(__taklalive__);

    consumer();



    semctl(semid, 0, IPC_RMID);



    return 0;
}

