#include <iostream>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <cmath>
#include "mlib.h"

using Eigen::MatrixXd;



void seq_gen(uint64 depth, std::vector<uint64> &seq)
{
    if (depth < 1) {
        std::cout << "Wrong depth specified! " << depth << std::endl;
        return;
    }

    seq.push_back(1);
    seq.push_back(2);

    for (uint64 n = 2; n <= depth; ++n) {
        for (int i = seq.size() - 1; i >= 0; i--) {
            std::vector<uint64>  to_insert;
            to_insert.push_back ((1 << n) + 1 - seq.at(i));
            seq.insert(seq.begin() + i + 1, to_insert[0]);
        }
    }
};


double get_thau(double L, double l, uint64 j, uint64 N)
{
    //return 1.0/((L + l)/2.0 + ((L - l)/2.0) * cos(M_PI*(2*j-1)/(2*N)));
    return 2.0/(L + l);
};


int main()
{
    uint64 depth = 15;
    matVec mat_vec = mat_vec_from_file("matrix.txt");
    std::cout << "Input matrix:\n" << mat_vec.mat << "\n\n";
    std::cout << "Right side:\n"   << mat_vec.vec << "\n\n";
    Eigen::VectorXd eig_vec = vec_from_file ("eigenvalues.txt");
    std::cout << "Matrix eigenvalues:\n" << eig_vec << "\n\n";

    std::vector<uint64> seq;
    seq_gen(depth, seq);

    std::cout << "Sequence (" << (1 << depth) << " numbers):\n";
    for (int i = 0; i < seq.size(); ++i) {
        std::cout << seq.at(i) << " ";
    }
    std::cout << std::endl;

    double L = eig_vec.maxCoeff();
    double l = eig_vec.minCoeff();
    std::cout << "minimum eigenvalue: " << l << "; maximum: " << L << "\n";

    Eigen::VectorXd u = Eigen::VectorXd::Zero(mat_vec.mat.rows());
    std::cout << "Starting from u = :\n" << u << "\n";
    for (int i = 0; i < seq.size(); ++i) {
        int j = seq.at(i);
        double thau = get_thau(L, l, j, (1 << depth));
        std::cout << "thau(" << j << ") = " << thau << "\n";
        u = u + thau * (mat_vec.mat * u - mat_vec.vec);
        std::cout << "u(" << j << ") = :\n" << u << "\n";
    }

    std::cout << "The result is:\n" << u << "\n";
    return 0;
};
