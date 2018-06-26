cmake_minimum_required(VERSION 3.10)
project(Basic)

set(CMAKE_CXX_STANDARD 11)

# opencv
set(OpenCV_DIR /usr/local/opencv3/share/OpenCV)
find_package(OpenCV 3.0 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIR})

# Ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

add_executable(Basic main.cpp)
target_link_libraries(Basic ${OpenCV_LIBS}  ${CERES_LIBRARIES})