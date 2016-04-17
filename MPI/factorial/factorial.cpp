#include <mpi.h>
#include <iostream>
#include <cstring>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[])
{
    long long int N_MAX = atol(argv[1]); 

    double dt = MPI::Wtime();
    MPI::Init(argc, argv);
    MPI::COMM_WORLD.Barrier();

    int size = MPI::COMM_WORLD.Get_size();
    int id = MPI::COMM_WORLD.Get_rank();

    if (id == 0) {
        std::cout << "Init time = " << MPI::Wtime() - dt  << std::endl;
    }
    dt = MPI::Wtime();

    long long int start = (id == 0) ? 1 : id * N_MAX / size;
    long long int end   = (id == (size - 1)) ? N_MAX : ((id+1) * N_MAX / size - 1);


    double sum  = 0.0;
    double fact_iter = 1.0;
    for (long long int i = start; i <= end; i++) {
        fact_iter *= i;
        sum  += 1.0 / fact_iter;
    }
    double fact = 1.0/fact_iter;

    // Da first one
    double res = sum;
    if (id == 0) {
        sum = 0;
        for (long long int i = size-1; i >= 1; i--) {
            double s = 0.0;
            double f = 0.0;
            MPI::COMM_WORLD.Recv((&s), 1, MPI::DOUBLE, i, 0);
            MPI::COMM_WORLD.Recv((&f), 1, MPI::DOUBLE, i, 1);
            sum *= f;
            sum += s;
        }
        res += fact * sum;
    }
    
    // Others
    if (id != 0) {
        MPI::COMM_WORLD.Send((&sum),  1, MPI::DOUBLE, 0, 0);
        MPI::COMM_WORLD.Send((&fact), 1, MPI::DOUBLE, 0, 1);
    }
    

    // Finishing, printing out the time
    MPI::COMM_WORLD.Barrier();
    
    if (id == 0) {
        std::cout << "Time = " << MPI::Wtime() - dt << std::endl;
        std::cout << "Result: " << res << std::endl;
    }

    MPI::Finalize();
    return 0;
}
