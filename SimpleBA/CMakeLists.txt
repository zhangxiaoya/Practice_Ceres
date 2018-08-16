cmake_minimum_required(VERSION 3.10)
project(SimpleBA)

set(CMAKE_CXX_STANDARD 11)

#Ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})

add_executable(SimpleBA main.cpp)
target_link_libraries(SimpleBA ${CERES_LIBRARIES})