#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H

void swap_columns(float** MATRIX, int col1, int col2);
void swap_rows(float** MATRIX, int row1, int row2, int n);
float determinant(float** MATRIX, float determ, int n);
float eval_det(float** MATRIX, int k1, int n, float * b );

#endif // HELP_FUNCTIONS_H
