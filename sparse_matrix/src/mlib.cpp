#include "mlib.h"


Eigen::MatrixXd from_file(std::string fname) {


};

void to_file (Eigen::MatrixXd mat, std::string fname) {
    if (mat.rows() != mat.cols()) {
        std::cerr << "The matrix is not square! (" << mat.rows() << "x" << mat.cols() << ")";
    }
    if (mat.rows() < 1) {
        std::cerr << "The matrix is too small! (" << mat.rows() << "x" << mat.cols() << ")";
    }

    std::ofstream file (fname.c_str(), std::ios::out);
    if (file.is_open()) {
        std::cout << "Successfully opened '" << fname << "' for writing...\n";
        file << mat.rows() << "\n";
        for (uint64 i = 0; i < mat.rows(); ++i)
            for (uint64 j = 0; j < mat.cols(); ++j)
                file << mat(i, j) << "\n";

        file.close();
        std::cout << "...Complete!\n";
    }

    else {
        std::cerr << "Unable to open file '" << fname << "'\n";
    }
};

Eigen::MatrixXd generate_matrix(int64 size) {
    Eigen::MatrixXd  mat(size, size);
    Eigen::MatrixXd Tmat(size, size);
    std::srand(std::time(0)); // use current time as seed for random generator

    for (uint64 i = 0; i < mat.rows(); ++i)
        for (uint64 j = 0; j < mat.cols(); ++j)
            mat(i, j) = double(std::rand() - (RAND_MAX / 2)) / 10000000.0;

    Tmat = mat.transpose();
    return Tmat*mat;
};

Eigen::MatrixXd generate_matrix_eigenvalues(Eigen::VectorXd) {

};

