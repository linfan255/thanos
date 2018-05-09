cmake_minimum_required(VERSION 2.8)
project(lf_server)

set(CMAKE_CXX_COMPILER "g++") # set the compiler

# debug mode or release mode
# debug mode can set the break point
# but release mode may be faster
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

# set compiler options
#   c++11, 03 optimization, multiple threads ...
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread")

# set bin directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set lib directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# set link directory
link_directories(${PROJECT_SOURCE_DIR}/lib)

# set include headers' directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# add subdirectories
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/unittest)
