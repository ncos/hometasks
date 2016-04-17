#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>    

#define ISIZE 1000
#define JSIZE 1000

int main(int argc, char **argv) {
  double a[ISIZE][JSIZE];
  int i, j;
  FILE *ff;

  // Initialization
  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE; ++j) {
      a[i][j] = 10 * i + j;
    }
  }

  clock_t t = clock();

  // Parallelize
  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE - 2; ++j) {
      a[i][j] = sin(0.00001 * a[i][j + 2]);
    }
  }

  t = clock() - t;
  printf("Time: %f\n", ((float)t)/CLOCKS_PER_SEC);

  ff = fopen("4.out", "w");
  for (i = 0; i < ISIZE; ++i) {
    for (j = 0; j < JSIZE; ++j) {
      fprintf(ff, "%f\n", a[i][j]);
    }
    fprintf(ff, "\n");
  }
  fclose(ff);
  return 0;
}

