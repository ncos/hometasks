
#ifndef CONFIH_H
#define CONFIG_H



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>


#define MAX_RETRIES 10
#define MEMSIZE 10000
#define TIMEOUT 10

#define __taklalive__ 4
#define __listalive__ 3
#define __fillCount__ 2
#define __emptyCount__ 1
#define __finishFlag__ 0



union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

struct package
{
    int size;
    unsigned char shm_buf[MEMSIZE + 1];
};

extern int shm_id;
extern int semid;
extern key_t key;

extern struct package *shm_package;

extern time_t start, end;
extern int t_left;


int initsem(key_t key, int nsems);

void set(int sem_num, int val);

void up(int sem_num); // A

void down(int sem_num); // D

void wait_nz(int sem_num);

void shm_init();

void err_exit(const char str[]);

void safe_up  (int sem_num, int alive_sem); // --sem_num if alive_sem is 1

void safe_down(int sem_num, int alive_sem); // --sem_num if alive_sem is 1

void waitone(int sem_num); // wait while sem_num is zero

void set_alive(int alive_sem);



#endif
