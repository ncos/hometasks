#include <iostream>
#include <eigen3/Eigen/Dense>
#include "mlib.h"
using Eigen::MatrixXd;

int main()
{
//    Eigen::MatrixXd mat = generate_matrix(10);
//    std::cout << mat << "\n";

//    to_file (mat, "matrix.txt");

    Eigen::MatrixXd mat = from_file("matrix.txt");
    std::cout << mat << "\n";

    return 0;
};
