#ifndef MLIB_H
#define MLIB_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <eigen3/Eigen/Dense>

typedef long long int int64;
typedef unsigned long long int uint64;

struct matVec {
    Eigen::MatrixXd mat;
    Eigen::VectorXd vec;
};

Eigen::VectorXd vec_from_file (std::string fname);
matVec mat_vec_from_file(std::string fname);



#endif
