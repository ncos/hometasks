#include <iostream>
#include <vector> 
#include "Sudoku.h"


typedef std::vector<std::vector<int> > arr_t;


int main()
{
    int N = 9;
    arr_t sudoku;
    
    sudoku.resize(N);

    for (int i = 0; i < N; i++)
    {
        sudoku[i].resize(N);
        for (int j = 0; j < N; j++)
        {
            sudoku[i][j] = 0;
        };
    };
    
    sudoku[2][2] = 3;
    sudoku[0][0] = 4;
    sudoku[1][2] = 9;
    sudoku[2][3] = 8;
    sudoku[5][4] = 2;
    sudoku[4][4] = 5;
    sudoku[7][1] = 6;
    sudoku[4][2] = 4;
    sudoku[5][4] = 1;
    sudoku[2][4] = 7;
    sudoku[3][5] = 6;
    sudoku[4][2] = 7;

    
    sudoku = Sudoku::matrix::solve(sudoku); //Solving sudoku "sudoku"
    for (int j = 0; j < N; j++)
    {
        for (int i = 0; i < N; i++)
        {
            std::cout<<sudoku[i][j]<<" ";
        };
        std::cout<<"\n";
    };
};

