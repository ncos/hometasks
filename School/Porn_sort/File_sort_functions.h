#ifndef FILE_SORT_FUNCTIONS_H
#define FILE_SORT_FUNCTIONS_H


#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

typedef long long int int64;
typedef unsigned long long int uint64;


void vclear(std::vector<int> &v);
    
int minnum(std::vector<int> &arr);

int64 fsize(std::ifstream myfile);

std::string generate_path(std::string &path, int &n);
    


#endif // FILE_SORT_FUNCTIONS_H

