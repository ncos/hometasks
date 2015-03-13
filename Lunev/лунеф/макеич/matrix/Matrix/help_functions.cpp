#include "help_functions.h"


//menyaem stolbci mestami.
void swap_columns(float** MATRIX, int col1, int col2)
{
   float* temp = MATRIX[col1];
   MATRIX[col1] = MATRIX[col2];
   MATRIX[col2] = temp;
}

//menyaem stroki mestami.
void swap_rows(float** MATRIX, int row1, int row2, int n)
{
   float temp;
   for(int i=0; i<n; ++i)
   {
      temp = MATRIX[i][row1];
      MATRIX[i][row1] = MATRIX[i][row2];
      MATRIX[i][row2] = temp;
   }

}
float determinant(float** MATRIX, float determ, int n)
{
   int sign = +1;
   int x,y;
   /*dopolnitel'naya matrica*/
   float **TempMatrix = new float*[n];
   for(x=0; x<n; ++x)
      TempMatrix[x] = new float[n];

   for(y=0; y<n; ++y)
      for(x=0; x<n; ++x)
         TempMatrix[x][y]=MATRIX[x][y];

   for(int i=0; i<n; ++i) //dvigaemsya vdol' diagonali
   {
      //ischem nenulevoi element na glavnoi diagonali ot (i,i) do (n,n)
      int nonull_x = 0, nonull_y = 0;
      bool success_finding_nonull_element = false;

      for(x = i; x<n; ++x)
         for(y = i; y<n; ++y)
            if(!success_finding_nonull_element && MATRIX[x][y])
            {
                nonull_x = x;
                nonull_y = y;
                success_finding_nonull_element = true;
            }

      if(!success_finding_nonull_element) //esli net nenulevogo elementa
      {
         determ = 0.0;
         return determ;
      }

      if(nonull_x != i)
      {
         swap_columns(MATRIX, i, nonull_x);
         sign = -sign;
      }

      if(nonull_y != i)
      {
         swap_rows(MATRIX, i, nonull_y, n);
         sign = -sign;
      }

      //now a[i][i] != 0  !!!
      determ *= MATRIX[i][i];
      for(x=n-1; x>=i; --x)
         MATRIX[x][i] /= MATRIX[i][i];
      //now a[i][i] == 1;

      //privodim k diagonal'nomu vidu(pervii stolbec):
      for (y=i+1; y<n; ++y)
         for (x=n-1; x>=i; --x)
            MATRIX[x][y] -= MATRIX[x][i]*MATRIX[i][y];

   }
   /*vovrashaem prejnie znacheniya elementov matrici*/
   for(y=0; y<n; ++y)
      for(x=0; x<n; ++x)
         MATRIX[x][y]=TempMatrix[x][y];


   determ *= sign;

   for(x=0; x<n; ++x)
   {
      delete (TempMatrix[x]);
   }

   delete (TempMatrix);


   return determ;
}


float eval_det(float** MATRIX, int k1, int n, float *b)
{
   /*dopolnitel'nii massiv dlya hraneniya zamenyaemogo stolbca*/
   float *bb = new float[n];
   int i,y;
   float dete=1.0;
   for(i=0; i<n; i++)
      bb[i]=0.0;



   for(y=0; y<n; y++)
   {
      bb[y]=MATRIX[k1][y];//zapominaem znachenie
      MATRIX[k1][y]=b[y];//zamenyaem
   }

   dete=determinant(MATRIX, dete, n);
   for(y=0; y<n; y++)//vozvrashaem ishodnie znacheniya elementov stolbca
      MATRIX[k1][y]=bb[y];


   delete(bb);

   return dete;
}

