
/**
        C++ Producer Consumer Problem by Anton Mitrokhin
*/


#include "config.h"

/*
** initsem() -- more-than-inspired by W. Richard Stevens' UNIX Network
** Programming 2nd edition, volume 2, lockvsem.c, page 295.
*/

int shm_id;
int semid;
key_t key;

struct package *shm_package;

time_t start, end;
int t_left;



void err_exit(const char str[])
{
    perror(str);

    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
        perror("shm remove_err");

    if (semctl(semid, 0, IPC_RMID) == -1)
        perror("sem remove_err");

    exit(-1);
}


int initsem(key_t key, int nsems)  /* key from ftok() */
{
    //int i;
    //union semun arg;
    //struct semid_ds buf;
    //struct sembuf sb;

    int semid = semget(key, nsems, IPC_CREAT | 0666);

    if(semid < 0)
    {
        perror("Error creating semaphore");
        exit(-1);
    }

    return semid;
}


void set(int sem_num, int val)
{
    union semun arg;
    arg.val = val;

    if(semctl(semid, sem_num, SETVAL, arg) < 0)
        err_exit("Can not SET");
}


void up(int sem_num) // A
{
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    if(semop(semid, &sb, 1) < 0)
        err_exit("Can not UP");
}


void safe_up(int sem_num, int alive_sem) // ++sem_num if alive_sem is 1
{
    struct sembuf sb[3];

    sb[0].sem_num = alive_sem;
    sb[0].sem_op = -1;
    sb[0].sem_flg = IPC_NOWAIT;

    //sb[1].sem_num = alive_sem;
    //sb[1].sem_op = 0;
    //sb[1].sem_flg = IPC_NOWAIT;

    sb[1].sem_num = sem_num;
    sb[1].sem_op = 1;
    sb[1].sem_flg = SEM_UNDO;

    sb[2].sem_num = alive_sem;
    sb[2].sem_op = 1;
    sb[2].sem_flg = 0;

    if(semop(semid, sb, 3) < 0)
        err_exit("Can not SAFE_UP");
}


void set_alive(int alive_sem)
{
    struct sembuf sb[2];

    sb[0].sem_num = alive_sem;
    sb[0].sem_op = 0;
    sb[0].sem_flg = IPC_NOWAIT;

    sb[1].sem_num = alive_sem;
    sb[1].sem_op = 1;
    sb[1].sem_flg = SEM_UNDO;


    if(semop(semid, sb, 2) < 0)
        exit(-1);
}


void waitone(int sem_num) // wait while sem_num is zero
{
    struct sembuf sb[2];

    sb[0].sem_num = sem_num;
    sb[0].sem_op = -1;
    sb[0].sem_flg = 0;

    sb[1].sem_num = sem_num;
    sb[1].sem_op = 1;
    sb[1].sem_flg = 0;

    if(semop(semid, sb, 2) < 0)
        err_exit("Can not WAIT WHILE ZERO");
}


void down(int sem_num) // D
{
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    if(semop(semid, &sb, 1) < 0)
        err_exit("Can not DOWN");
}


void safe_down(int sem_num, int alive_sem) // --sem_num if alive_sem is 1
{
    struct sembuf sb[3];

    sb[0].sem_num = alive_sem;
    sb[0].sem_op = -1;
    sb[0].sem_flg = IPC_NOWAIT;

    sb[1].sem_num = sem_num;
    sb[1].sem_op = -1;
    sb[1].sem_flg = SEM_UNDO;

    sb[2].sem_num = alive_sem;
    sb[2].sem_op = 1;
    sb[2].sem_flg = 0;

    if(semop(semid, sb, 3) < 0)
        err_exit("Can not SAFE_DOWN");
}

/*
void exclude_up(int sem_num, int alive_me)
{
    struct sembuf sb[3];

    sb[0].sem_num = alive_me;
    sb[0].sem_op = -1;
    sb[0].sem_flg = IPC_NOWAIT;

    sb[0].sem_num = alive_me;
    sb[0].sem_op = 0;
    sb[0].sem_flg = IPC_NOWAIT;

    sb[1].sem_num = sem_num;
    sb[1].sem_op = 1;
    sb[1].sem_flg = SEM_UNDO;


    if(semop(semid, sb, 2) < 0)
        err_exit("Can not SAFE SET ONE");
}
*/

void wait_nz(int sem_num)
{
    struct sembuf sb;
    sb.sem_num = sem_num;
    sb.sem_op =  0;
    sb.sem_flg = SEM_UNDO;
    if(semop(semid, &sb, 1) < 0)
        err_exit("Can not WAIT");
}


void shm_init()
{
    if((shm_id = shmget( key, sizeof(package) , 0644 | IPC_CREAT )) < 0)
        err_exit("Can not init SHM");


	/* Подключить общий сегмент к произвольному адресу */
	if((shm_package = (package *) shmat(shm_id, NULL, 0)) == NULL)
        err_exit("Can not connect to SHM");
}

