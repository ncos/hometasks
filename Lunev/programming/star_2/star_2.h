
#ifndef STAR2_H
#define STAR2_H


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>



int i;
int connection;
char byte;

sigset_t sigset_pipe, sigset_alarm, sigset_usr1, sigset_usr2, sigset_child;

int senderpid, receiverpid;













#endif
