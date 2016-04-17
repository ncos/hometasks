#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>    
#include <omp.h>

#define ISIZE 1000
#define JSIZE 1000

int main(int argc, char **argv) {
  double *a = new double[ISIZE*JSIZE];
  double *A = new double[ISIZE*JSIZE];

  FILE *ff;

  int nth   = 1;
  if (argc == 2) nth = atoi(argv[1]);

  omp_set_dynamic(0);
  omp_set_num_threads(nth);

  // Initialization
  for (int i = 0; i < ISIZE; ++i) {
    for (int j = 0; j < JSIZE; ++j) {
      a[i*JSIZE+j] = 10 * i + j;
    }
  }  
  for (int i = 0; i < ISIZE; ++i) {
    A[i*JSIZE+JSIZE-2] = 10 * i + JSIZE-2;
    A[i*JSIZE+JSIZE-1] = 10 * i + JSIZE-1;
  }

  double t = omp_get_wtime();

  for (int k = 0; k < 1000; ++k){
  // Parallelize
  #pragma omp parallel for collapse(2)
  for (int j = 0; j < JSIZE - 2; ++j) {
    for (int i = 0; i < ISIZE; ++i) {
      A[i*JSIZE+j] = sin(0.00001 * a[i*JSIZE+j+2]);
    }
  }
  }

  t = omp_get_wtime() - t;
  printf("Time: %f\n", t);

  ff = fopen("p4.out", "w");
  for (int i = 0; i < ISIZE; ++i) {
    for (int j = 0; j < JSIZE; ++j) {
      fprintf(ff, "%f\n", A[i*JSIZE+j]);
    }
    fprintf(ff, "\n");
  }
  fclose(ff);

  delete [] a;
  delete [] A;
  return 0;
}

