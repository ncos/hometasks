#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "File_sort_functions.h"
#include "Merge.h"
#include "Divide.h"

int dsize = 100;  //How many integers per file
std::string input_path  = "/home/ncos/Desktop/input_test_file";
std::string output_path = "/home/ncos/Desktop/output_test_file";
std::string buffer_path = "/home/ncos/Desktop/sorting/";


int main()
{    
    divide_and_sort(input_path, buffer_path, dsize);


        //int N = divide_and_sort(f, buffer_path, dsize);
        //std::cout << "divided.\n";
        //std::cout << "\n";
        //merge(N, output_path, buffer_path);
        //std::cout<<"merged.\n";



    
    return 0;
};
   
    
