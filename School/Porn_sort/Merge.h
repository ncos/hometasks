#ifndef MERGE_H
#define MERGE_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <cstring>
#include <string>
#include <vector>
#include "File_sort_functions.h"

std::vector<int> comparison_line(int N, std::vector<int> &buf, std::vector<int> &place, char from_where[64]);
    
void merge(int N, char *out_path, char buf_path[64]);


#endif // MERGE_H
