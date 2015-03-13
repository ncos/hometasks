
/**
        C++ Producer Consumer Problem by Anton Mitrokhin
*/


#include "config.h"

void putItemIntoBuffer(unsigned char buf [MEMSIZE + 1], int size)
{
    for(int i = 0; i < MEMSIZE + 1; i++)
        (shm_package->shm_buf[i]) = buf[i];
    (shm_package->size) = size;
}




void producer(int semid, int fd)
{
    shm_init();

    unsigned char buf [MEMSIZE + 1];
    memset(buf, '\0', MEMSIZE + 1 );

    long int size = 0, cnt = 0;
    //while (semctl(semid, __finishFlag__, GETVAL, 0) == 0)
    while(1)
    {
        size = read(fd, buf, MEMSIZE);

        cnt++;
        safe_down(__emptyCount__, __listalive__);

        putItemIntoBuffer(buf, size );
        printf("Packet %ld: %d\n", cnt,   semctl(semid, __listalive__, GETVAL, 0)  );

        safe_up(__fillCount__, __listalive__);
        memset(buf, '\0', MEMSIZE );
    }

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
        err_exit("initsem");


    set_alive(__taklalive__);
    //safe_up(__emptyCount__, __taklalive__);
    set(__emptyCount__, 1);
    set(__finishFlag__, 0);
    set(__fillCount__ , 0);
    //set(__taklalive__ , 0);
    //set(__listalive__ , 0);


    //while(semctl(semid, __finishFlag__, GETVAL, 0) != 0 ) set(__fillCount__, 0);
    /*time(&start);
    while ( semctl(semid, __finishFlag__, GETVAL, 0) != 0 )
    {
        time(&end);
        t_left = TIMEOUT-difftime (end, start);
        printf("%d (s)\r", t_left);


        if( semctl(semid, __fillCount__, GETVAL, 0) == 1 ) down(__fillCount__);
        if( semctl(semid, __taklalive__, GETVAL, 0) == 0 ) up(__taklalive__);

        if(t_left <= 0) {errno = ETIME; err_exit("unable to connect");};
    }
    if( semctl(semid, __taklalive__, GETVAL, 0) == 0 ) up(__taklalive__);
*/

//up(__taklalive__);
//set(__fillCount__ , 0);
//wait_nz(__finishFlag__);


    //while ( semctl(semid, __listalive__, GETVAL, 0) != 1 ) {};
    waitone(__listalive__);

    printf("__finishFlag__ = %d\n", semctl(semid, __finishFlag__, GETVAL, 0)  );
    printf("__emptyCount__ = %d\n", semctl(semid, __emptyCount__, GETVAL, 0)  );
    printf("__fillCount__  = %d\n", semctl(semid, __fillCount__ , GETVAL, 0)  );
    printf("__taklalive__  = %d\n", semctl(semid, __taklalive__ , GETVAL, 0)  );
    printf("__listalive__  = %d\n", semctl(semid, __listalive__ , GETVAL, 0)  );

//getchar();


    int fd = open (argv[1], O_RDONLY);
    producer(semid, fd);



    semctl(semid, 0, IPC_RMID);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}


