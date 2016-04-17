#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector> 


typedef std::vector<std::vector<int> > arr_t;  //It'll be our sudoku grid



namespace Sudoku
{
    class matrix
    { 
        private:
            static bool can_place_in_a_line(arr_t matrix, int j, int elem, int N);
            static bool can_place_in_a_column(arr_t matrix, int i, int elem, int N);
            static bool can_place_in_a_square(arr_t matrix, int i, int j, int elem, int N);
            static bool can_place_here(arr_t matrix, int i, int j, int elem, int N);
        public:
            static arr_t solve(arr_t matrix);
    };         
};


#endif // SUDOKU_H


                
        

            



          

