
#ifndef CONFIH_H
#define CONFIG_H



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include <iostream>       // std::cerr
#include <typeinfo>       // operator typeid
#include <exception>      // std::exception
#include <stdexcept>



#define GREETPIPE_NAME "/tmp/my_named_greeting_pipe"
#define BUFSIZE 90001
#define PATH_MAX 128
#define COMM_FIFO_LOC "/tmp/talker-%d-comm-fifo"
#define SYNC_FIFO_TL_LOC "/tmp/talker-tl-%d-sync-fifo"
#define SYNC_FIFO_LT_LOC "/tmp/talker-lt-%d-sync-fifo"
#define TIMEOUT 10


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[1m\x1B[31m"
#define KGRN  "\x1B[1m\x1B[32m"
#define KYEL  "\x1B[1m\x1B[33m"
#define KBLU  "\x1B[1m\x1B[34m"
#define KMAG  "\x1B[1m\x1B[35m"
#define KCYN  "\x1B[1m\x1B[36m"
#define KWHT  "\x1B[1m\x1B[37m"


#define __WARN_(...) {printf(KYEL); printf("[%d]: ", getpid()); printf(__VA_ARGS__); printf(KNRM);}
#define __ERR_ (...) {printf(KRED); printf("[%d]: ", getpid()); printf(__VA_ARGS__); printf(KNRM);}
#define __NTFY_(...) {printf(KBLU); printf("[%d]: ", getpid()); printf(__VA_ARGS__); printf(KNRM);}

#define __WARN(...) {std::cerr << KYEL << getpid() << ": " << __VA_ARGS__ << KNRM;}
#define __ERR (...) {std::cerr << KRED << getpid() << ": " << __VA_ARGS__ << KNRM;}
#define __NTFY(...) {std::cerr << KBLU << getpid() << ": " << __VA_ARGS__ << KNRM;}


#define __CHECK(str) {if ((str) < 0) {std::cerr << KRED << "Error_" << KNRM; fflush(stderr); exit(EXIT_FAILURE);};}

enum SYNQ_SYG { START_SQ, STOP_SQ, PENDING_SQ, READY_SQ, READ_SS, WRITE_SS };


#endif
