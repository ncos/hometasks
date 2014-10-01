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

Eigen::MatrixXd from_file(std::string fname);

void to_file (Eigen::MatrixXd mat, std::string fname);

Eigen::MatrixXd generate_matrix(int64 size);

Eigen::MatrixXd generate_matrix_eigenvalues(Eigen::VectorXd);



#endif
