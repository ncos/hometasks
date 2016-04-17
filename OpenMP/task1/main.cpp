#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <limits>

#include <math.h>
#include <omp.h>

#define vperf

double h;
int R = 10;
double eps = 0.000001;
double b_min = 0.0;
double b_max = 1.0;
double b_step = 0.1;



double dist(double *x, int len) {
    double acc = 0.0;
    for(int i = 0; i <= len; ++i) {
        acc += abs(x[i]);
    }
    return acc;
}

double f(double y) {
    return exp(y);
}

double run(double b, int N, int nth) {
    double *y  = new double[N + 1];
    double *dy = new double[N + 1];

    dy[0] = 0.0; 
    dy[N] = 0.0;

    double **A = new double*[R];
    double **B = new double*[R];
    double **C = new double*[R];
    double **G = new double*[R];


    for(int i = 0; i <= N; ++i) {
        y[i] = 1 + (b - 1) * i / N;
    }

    for (int i = 0; i < R; ++i) {
        A[i]  = new double[N+1];
        B[i]  = new double[N+1];
        C[i]  = new double[N+1];
        G[i]  = new double[N+1];
    }

    for(int i = 0; i < R; ++i) {
        B[i][0] = 1;
        B[i][N] = 1;
    }

    int s;
    omp_set_num_threads(nth);
    double time = omp_get_wtime();
 
    do {
        #pragma omp parallel for
        for(int i = 1; i < N; ++i) {
            B[0][i] = (-2.0/(h * h) - 10 * exp(y[i]) / 12);
            A[0][i] = ( 1.0/(h * h) - exp(y[i-1]) / 12);
            C[0][i] = ( 1.0/(h * h) - exp(y[i+1]) / 12);
            G[0][0] = y[0] - 1.0;
            G[0][N] = y[N] - b;
            G[0][i] = (y[i + 1] - 2 * y[i] + y[i - 1]) / (h*h) - 
                      (f(y[i + 1]) + 10 * f(y[i]) + f(y[i - 1])) / 12;
        }

        for(int j = 1; j < R; ++j) {
            s = pow(2, j);
            #pragma omp parallel for
            for(int i = s; i < N; i += s) {
                A[j][i] = - A[j - 1][i] * A[j - 1][i - s/2] / B[j - 1][i - s/2];
                C[j][i] = - C[j - 1][i] * C[j - 1][i + s/2] / B[j - 1][i + s/2];
                B[j][i] =   B[j - 1][i] - A[j - 1][i] * C[j - 1][i - s/2] / B[j - 1][i - s/2] - 
                            C[j - 1][i] * A[j - 1][i + s/2] / B[j - 1][i + s/2];
                G[j][i] =   G[j - 1][i] - A[j - 1][i] * G[j - 1][i - s/2] / B[j - 1][i - s/2] - 
                            C[j - 1][i] * G[j - 1][i + s/2] / B[j - 1][i + s/2];
            }
        }

            
        dy[N/2] = G[R-1][N/2] / B[R-1][N/2];
        dy[N/4] = (G[R-2][N/4]   - C[R-2][N/4]   * dy[N/2])  / B[R-2][N/4];
        dy[N*3/4] = (G[R-2][N*3/4] - A[R-2][N*3/4] * dy[N/2] ) / B[R-2][N*3/4];
            

        for(int j = R-3; j >= 0; j--) {
            s = pow(2, j);
            #pragma omp parallel for
            for(int i = s; i < N; i += 2 * s)
                dy[i] = (G[j][i] - C[j][i] * dy[i+s] - A[j][i] * dy[i-s]) / B[j][i];
        }
        #pragma omp parallel for
        for(int i = 0; i <= N; i++)
            y[i] -= dy[i];
    }
    while (dist(dy, N) > eps);
    
    time = omp_get_wtime() - time;

#ifndef perf
    std::cout << "Time:\t" << time << std::endl;
    std::cout << "X" << "\t\t" << "Y" << std::endl;
    std::cout.precision(7);
    for (int i = 0; i <= N; ++i)
       std::cout << std::fixed << ((double) i / N) << "; " << y[i] << std::endl;
#else
    std::cout << time << "; ";
#endif

    for (int i = 0; i < R; i++) {
        delete A[i];
        delete B[i];
        delete C[i];
        delete G[i];
    }
    delete A;
    delete B;
    delete C;
    delete G;
    delete y;
    delete dy;
}


int main(int argc, char** argv)
{
    int nth = 1;
    if (argc != 5) {
        std::cout << "Usage:" << argv[0] << " <nthreads> <b_min> <b_max> <b_step>\n";
        return 0;
    }
    else {
        nth = atoi(argv[1]);
        b_min   = atof(argv[2]);
        b_max   = atof(argv[3]);
        b_step  = atof(argv[4]);

        if ((nth < 1) || (b_min >= b_max) || (b_step <= 0)) {
            std::cout << "Invalid parameters!\n";
            return 0;  
        }
    }

    int N = pow(2, R);
    h = 1.0/N;

    for (double b = b_min; b <= b_max; b += b_step)
    {
#ifndef perf
        std::cout << std::endl << std::endl << "b = " << b << std::endl;
#endif           
        run(b, N, nth);
    }

    std::cout << std::endl;
    return 0;
}

