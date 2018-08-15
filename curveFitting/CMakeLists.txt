cmake_minimum_required(VERSION 3.10)
project(curveFitting)

set(CMAKE_CXX_STANDARD 11)

#ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

add_executable(curveFitting main.cpp)
target_link_libraries(curveFitting ${CERES_LIBRARIES})