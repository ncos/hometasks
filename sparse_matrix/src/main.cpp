#include <iostream>
#include <iomanip>
#include <cstring>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <cmath>
#include "mlib.h"

using Eigen::MatrixXd;


static inline void loadbar(std::string str_, unsigned int x, unsigned int n, unsigned int w = 50) {
    if ( (x != n) && (x % (n/100+1) != 0) ) return;

    float ratio  =  x/(float)n;
    int   c      =  ratio * w;

    std::cout << str_;
    std::cout << std::setw(3) << (int)(ratio*100) << "% [";
    for (int x=0; x<c; x++) std::cout << "=";
    for (int x=c; x<w; x++) std::cout << " ";
    std::cout << "]\r" << std::flush;
};



void seq_gen(uint64 depth, std::vector<uint64> &seq)
{
    if (depth < 1) {
        std::cout << "Wrong depth specified! " << depth << std::endl;
        return;
    }

    seq.push_back(1);
    seq.push_back(2);

    for (uint64 n = 2; n <= depth; ++n) {
        loadbar("Generating sequence: ", n, depth);
        for (int i = seq.size() - 1; i >= 0; i--) {
            std::vector<uint64>  to_insert;
            to_insert.push_back ((1 << n) + 1 - seq.at(i));
            seq.insert(seq.begin() + i + 1, to_insert[0]);
        }
    }
    std::cout << "\n";
};


double get_thau(double L, double l, uint64 j, uint64 N)
{
    return -1.0/((L + l)/2.0 + ((L - l)/2.0) * cos(M_PI*(2*j-1)/(2*N)));
};


int main()
{
    uint64 depth = 18;
    matVec mat_vec = mat_vec_from_file("matrix.txt");
    Eigen::VectorXd eig_vec = vec_from_file ("eigenvalues.txt");

    std::cout << "Input matrix:\n" << mat_vec.mat << "\n\n";
    std::cout << "Right side:\n"   << mat_vec.vec.transpose() << "\n\n";
    std::cout << "Matrix eigenvalues:\n" << eig_vec.transpose() << "\n\n";
    std::cout << "Sequence (" << (1 << depth) << " numbers):\n";
    double L = eig_vec.maxCoeff();
    double l = eig_vec.minCoeff();
    std::cout << "minimum eigenvalue: " << l << "; maximum: " << L << "\n";

    std::vector<uint64> seq;
    seq_gen(depth, seq);

    Eigen::VectorXd u = Eigen::VectorXd::Zero(mat_vec.mat.rows());

    for (int i = 0; i < seq.size(); ++i) {
        int j = seq.at(i);
        double thau = get_thau(L, l, j, (1 << depth));
        u = u + thau * (mat_vec.mat * u - mat_vec.vec);
        loadbar("Calculating: ", i + 1, seq.size());
    }

    std::cout << "\n\n";
    std::cout << "The result is:\n[";
    for (int i = 0; i < u.rows() - 1; ++i) {
        std::cout << u[i] << ", ";
    }
    std::cout << u[u.rows() - 1] << "]\n";
    return 0;
};
