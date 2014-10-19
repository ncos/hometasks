project(SparseMatrix)
cmake_minimum_required(VERSION 2.6.2)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Eigen3 REQUIRED)
include_directories(EIGEN3_INCLUDE_DIR)

set(SRCS src/main.cpp src/mlib.cpp src/mlib.h)
add_executable(spmatrix ${SRCS})

set(SRCS src/matgen.cpp)
add_executable(matgen ${SRCS})
