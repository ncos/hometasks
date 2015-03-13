#include <stdio.h>
#include "deterthread.h"
#include "help_functions.h"

void DeterThread::StartCalculation(float **A,float main_det,int N,float * B,int k, float *result,int thread_nr)
{

   //sohranyaem paramentri.
   m_A = A;
   m_MainDet = main_det;
   m_N = N;
   m_B = B;
   m_k = k;
   m_result = result;
   m_thread_nr = thread_nr;

   //startuem tred, funkciya run budet vipolnyat'sya v otdel'nom trede
   this->start();
}

void DeterThread::run()
{

   printf("THREAD [%d] Started\n",m_thread_nr);


   float x_res = 0.0;

   //schitaem determinant s zamenennim stolbcom.
   float _det = eval_det(m_A,m_k,m_N,m_B);

   //vichislyaem reshenie sistemi.
   x_res = _det / m_MainDet;

   if(x_res==-0.0)
   {
     x_res = x_res * -1.0;
   }

   //sohranyaem rezul'tat
   *m_result = x_res;

   printf("THREAD [%d] Ended\n",m_thread_nr);

}
