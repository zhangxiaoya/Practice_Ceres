cmake_minimum_required(VERSION 3.10)
project(NumbericDerivatives)

set(CMAKE_CXX_STANDARD 11)

# Ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

add_executable(NumbericDerivatives main.cpp)
target_link_libraries(NumbericDerivatives ${CERES_LIBRARIES})