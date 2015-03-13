#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>

typedef long long int int64;
typedef unsigned long long int uint64;

using Eigen::MatrixXd;



void vec_to_file (Eigen::VectorXd vec, std::string fname) {
    std::ofstream file (fname.c_str(), std::ios::out);
    if (file.is_open()) {
        std::cout << "Successfully opened '" << fname << "' for writing...\n";
        file << vec.rows() << "\n";
        for (uint64 i = 0; i < vec.rows(); ++i)
            file << vec(i) << "\n";

        file.close();
        std::cout << "...Complete!\n";
    }

    else {
        std::cerr << "Unable to open file '" << fname << "'\n";
    }
};


void mat_vec_to_file (Eigen::MatrixXd mat, Eigen::VectorXd vec, std::string fname) {
    if (mat.rows() != mat.cols()) {
        std::cerr << "The matrix is not square! (" << mat.rows() << "x" << mat.cols() << ")";
    }
    if (mat.rows() < 2) {
        std::cerr << "The matrix is too small! (" << mat.rows() << "x" << mat.cols() << ")";
    }
    if (mat.rows() != vec.rows()) {
        std::cerr << "The matrix and vector sizes are different! (" << mat.rows() << " != " << vec.rows() << ")";
    }

    std::ofstream file (fname.c_str(), std::ios::out);
    if (file.is_open()) {
        std::cout << "Successfully opened '" << fname << "' for writing...\n";
        file << mat.rows() << "\n";
        for (uint64 i = 0; i < mat.rows(); ++i)
            for (uint64 j = 0; j < mat.cols(); ++j)
                file << mat(i, j) << "\n";
        for (uint64 i = 0; i < vec.rows(); ++i)
            file << vec(i) << "\n";

        file.close();
        std::cout << "...Complete!\n";
    }

    else {
        std::cerr << "Unable to open file '" << fname << "\n";
    }
};

Eigen::MatrixXd rotation_matrix(int64 size, int64 ax1, int64 ax2, double angle) {
    Eigen::MatrixXd mat(size, size);
    mat.setIdentity();
    if (ax1 == ax2) {
        std::cerr << "ax1 == ax2 in rotation_matrix call!\n";
        return mat;
    }
    if (ax1 >= size || ax2 >= size) {
        std::cerr << "ax1 or ax2 > size in rotation_matrix call!\n";
        return mat;
    }
    mat(ax1, ax1) =   cos(angle);
    mat(ax2, ax2) =   cos(angle);
    mat(ax1, ax2) = - sin(angle);
    mat(ax2, ax1) =   sin(angle);


    return mat;
};

Eigen::MatrixXd get_random_rotation(int64 size) {
    Eigen::MatrixXd mat(size, size);
    mat.setIdentity();

    for (int i = 0; i < size - 1; ++i) {
        for (int j = i + 1; j < size; ++j) {
            double angle = (rand() % int(2 * M_PI * 100)) / 100.0 - M_PI;
            mat *= rotation_matrix(size, i, j, angle);
        }
    }

    return mat;
};


Eigen::MatrixXd generate_matrix(int64 size) {
    Eigen::MatrixXd  mat(size, size);
    Eigen::MatrixXd Tmat(size, size);

    for (uint64 i = 0; i < mat.rows(); ++i)
        for (uint64 j = 0; j < mat.cols(); ++j)
            mat(i, j) = double(std::rand() - (RAND_MAX / 2)) / 10000000.0;

    Tmat = mat.transpose();
    return Tmat*mat;
};

struct matEig {
    Eigen::MatrixXd mat;
    Eigen::VectorXd eig;
};

matEig generate_matrix_eigenvalues(int64 size) {
    matEig mat_eig;
    mat_eig.mat = Eigen::MatrixXd (size, size);
    mat_eig.eig = Eigen::VectorXd (size);

    std::srand(std::time(0)); // use current time as seed for random generator
    for (uint64 i = 0; i < mat_eig.mat.rows(); ++i) {
        mat_eig.mat(i, i) = double(std::rand()) / 100000000.0;
        mat_eig.eig(i) = mat_eig.mat(i, i);
    }

    Eigen::MatrixXd rot = get_random_rotation(size);
    mat_eig.mat = rot * mat_eig.mat * rot.transpose();

    return mat_eig;
};


void print_mat_wolfram(Eigen::MatrixXd mat) {
    std::cout << "Matrix:\n";
    std::cout << "{";
    for(int i = 0; i < mat.rows(); ++i) {
        std::cout << "{";
        for(int j = 0; j < mat.cols(); ++j) {
            std::cout << mat(i, j);
            if (j != mat.rows() - 1) std::cout << ", ";
        }
        if (i != mat.rows() - 1) std::cout << "},\n";
    }
    std::cout << "}}\n";
};


int main() {
    uint64 size = 10;
    matEig mat_eig = generate_matrix_eigenvalues(size);
    Eigen::VectorXd answer_vec (size);
    std::srand(std::time(0)); // use current time as seed for random generator
    for (uint64 i = 0; i < size; ++i)
        answer_vec(i) = double(std::rand()) / 100000000.0;

    Eigen::VectorXd d_vec = mat_eig.mat * answer_vec;

    std::cout << "\n";
    print_mat_wolfram(mat_eig.mat);
    std::cout << "Eigen:\n[" << mat_eig.eig.transpose() << "]\n\n";
    std::cout << "The right side vector:\n[" << d_vec.transpose() << "]\n\n";
    std::cout << "The correct answers are:\n[" << answer_vec.transpose() << "]\n\n";

    mat_vec_to_file (mat_eig.mat, d_vec, "matrix.txt");
    vec_to_file (mat_eig.eig, "eigenvalues.txt");
    vec_to_file (answer_vec, "correct_answers.txt");

    return 0;
};
