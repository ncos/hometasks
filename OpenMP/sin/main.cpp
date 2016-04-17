#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <math.h>
#include <omp.h>


typedef  long long unsigned int llui;

double f(double x) {
    return (x != 0.0) ? sin(1/x) : 0;
}

int main(int argc, char **argv) {
    std::cout << "Integrate sin(1/x)!!!\n";

    int nth   = 1;
    double a  = -1;
    double b  = 1;
    double dx = 0.0001;

    if (argc != 5) {
        std::cout << "Usage:" << argv[0] << " <nthreads> <a> <b> <dx>\n";
        std::cout << "Using default values: nthreads = " << nth 
                  << ", a = " << a << ", b = " << b << ", dx = " << dx << "\n";
    }
    else {
        nth = atoi(argv[1]);
        a   = atof(argv[2]);
        b   = atof(argv[3]);
        dx  = atof(argv[4]);

        std::cout << "Launching with: nthreads = " << nth 
                  << ", a = " << a << ", b = " << b << ", dx = " << dx << "\n";
        if ((nth < 1) || (a >= b) || (dx <= 0)) {
            std::cout << "Invalid parameters!\n";
            return 0;  
        }
    }

    omp_set_dynamic(0);
    omp_set_num_threads(nth);

    double result = 0.0;
    llui iters = (int)((b - a) / dx);

    #pragma omp parallel for reduction (+:result)
    for (llui i = 0; i < iters; i++) {
       result += dx * f(dx*i);
    }

    printf("Result = %f\n", result);

    return 0;
}
