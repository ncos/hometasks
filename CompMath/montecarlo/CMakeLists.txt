project(montecarlo)
cmake_minimum_required(VERSION 2.6.2)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++0x")
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Boost COMPONENTS REQUIRED)
message("Include dirs of boost: " ${Boost_INCLUDE_DIRS} )
message("Libs of boost: " ${Boost_LIBRARIES} )

include_directories(
    ${Boost_INCLUDE_DIRS}
)

set(SRCS src/montecarlo.cpp)
add_executable(montecarlo ${SRCS})


target_link_libraries(montecarlo
    ${Boost_LIBRARIES}
)
