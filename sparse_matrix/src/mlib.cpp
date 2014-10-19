#include "mlib.h"




Eigen::VectorXd vec_from_file (std::string fname) {
    std::ifstream file (fname.c_str(), std::ios::in);
    Eigen::VectorXd vec;

    if (file.is_open()) {
        std::cout << "Successfully opened '" << fname << "' for reading...\n";
        uint64 n;
        file >> n;
        std::cout << "Reading a vector of size " << n << "...\n";
        vec = Eigen::VectorXd(n);
        for (uint64 i = 0; i < n; ++i)
            file >> vec(i);

        file.close();
        std::cout << "...Complete!\n";
    }

    else {
        std::cerr << "Unable to open file '" << fname << "'\n";
    }

    return vec;
};


matVec mat_vec_from_file(std::string fname) {
    std::ifstream file (fname.c_str(), std::ios::in);
    matVec mat_vec;

    if (file.is_open()) {
        std::cout << "Successfully opened '" << fname << "' for reading...\n";
        uint64 n;
        file >> n;
        std::cout << "Reading a matrix of size " << n << "x" << n << "...\n";
        mat_vec.mat = Eigen::MatrixXd(n, n);
        mat_vec.vec = Eigen::VectorXd(n);
        for (uint64 i = 0; i < n; ++i)
            for (uint64 j = 0; j < n; ++j) {
                file >> mat_vec.mat(i, j);
            }
        std::cout << "Reading a vector of size " << n << "...\n";
        for (uint64 i = 0; i < n; ++i)
            file >> mat_vec.vec(i);

        file.close();
        std::cout << "...Complete!\n";
    }

    else {
        std::cerr << "Unable to open file '" << fname << "'\n";
    }

    return mat_vec;
};
