#include "Sudoku.h"



bool Sudoku::matrix::can_place_in_a_line(arr_t matrix, int j, int elem, int N) 
//"True" if we can place "elem" in line j, otrerwise - "False"
{
    for (int i = 0; i < N; i++)
    {
        if (matrix[i][j] == elem)
        {
            return false;
        };
    };   
    return true;
};



bool Sudoku::matrix::can_place_in_a_column(arr_t matrix, int i, int elem, int N)
//"True" if we can place "elem" in colomn i, otrerwise - "False"
{
    for (int j = 0; j < N; j++)
    {
        if (matrix[i][j] == elem)
        {
            return false;
        };
    };    
    return true;
};


bool Sudoku::matrix::can_place_in_a_square(arr_t matrix, int i, int j, int elem, int N)
//"True" if we can place "elem" in a square 3x3, where "elem" is situated, otrerwise - "False"
{   
    int k, t;
    for (k = 0; k < N; k = k + 3)
    {
        for (t = 0; t < N; t = t + 3)
        {
            if ((t <= j)&&(j < t+3))
            {
                break;
            };
        };
        
        if ((k <= i)&&(i < k+3))
        {
            break;
        };
    };
        
    for (int l = k; l < k + 3; l++)
    {
        for (int m = t; m < t + 3; m++)
        {
            if (matrix[l][m] == elem)
            {
                return false;
            };
        };
    };
    return true;
};



bool Sudoku::matrix::can_place_here(arr_t matrix, int i, int j, int elem, int N)
//Can we put "elem" in this cell? 
{
    if (Sudoku::matrix::can_place_in_a_line(matrix, j, elem, N))
    {
        if (Sudoku::matrix::can_place_in_a_column(matrix, i, elem, N))
        {
            if (Sudoku::matrix::can_place_in_a_square(matrix, i, j, elem, N))
            {
                return true;
            };
        };
    };
    return false;
};



arr_t Sudoku::matrix::solve(arr_t matrix)
//This function solves sudoku
{   
    arr_t answer = matrix;
    int N = matrix.size(); //Size of sudoku matrix
    
    for (int j = 0; j < N; j++) //In each line
    {
        for (int i = 0; i < N; i++) //For each element
        {
            if (matrix[i][j] == 0) //If this cell is empty(0 means empty)
            {
                for (int elem = 1; elem <= 9; elem++) //Trying to put appropriate number
                {
                    if (Sudoku::matrix::can_place_here(matrix, i, j, elem, N))
                    {
                        arr_t matrix_ = matrix;
                        matrix_[i][j] = elem; //Making a new sudoku grid
                        matrix_ = solve(matrix_); //And use the same function "solve" to solve sudoku
                        if (matrix_[0][0] != 49) //But if we'd chosen a wrong way - we won't do this branch anymore
                        {
                            return matrix_;
                        };
                    }; 
                };  
                    
                arr_t matrix_ = matrix;
                matrix_[0][0] = 49; //If there is no such a number the cell to be filled with
                return matrix_;                                 
            };
        };
    };
    
    return answer;    
};
                        
