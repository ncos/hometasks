// Stancard C/C++ headers
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>

// MPI includes
#include <mpi.h>

//#define USE_CLUDGE
#define MSG_BUFF_SIZE 32



void sleepy_print(int id, std::string s, char *msg) {
#ifdef USE_CLUDGE
    sleep(0.2);
#endif // USE_CLUDGE
    std::cout << "(" << id << "): " << s << msg << std::endl;
#ifdef USE_CLUDGE
    sleep(0.2);
#endif // USE_CLUDGE
}

int main(int argc, char* argv[])
{
    MPI::Init(argc, argv);
    MPI::Status status;
    int size = MPI::COMM_WORLD.Get_size();
    int id = MPI::COMM_WORLD.Get_rank();

    // Da first one
    if (id == 0) {
        char msg[MSG_BUFF_SIZE] = "ping...";
        sleepy_print(id, "Sending: ", msg);
        MPI::COMM_WORLD.Send((msg), MSG_BUFF_SIZE, MPI::CHAR, 1, 0);
    }

    if (id != 0) {
        char msg[MSG_BUFF_SIZE];
        MPI::COMM_WORLD.Recv(msg, MSG_BUFF_SIZE, MPI::CHAR, id - 1, 0, status);
        sleepy_print(id, "Sending: ", msg);
        if (id + 1 < size) MPI::COMM_WORLD.Send(msg, MSG_BUFF_SIZE, MPI::CHAR, id + 1, 0);
    }


    MPI::COMM_WORLD.Barrier();
    MPI::Finalize();

    return 0;
}
