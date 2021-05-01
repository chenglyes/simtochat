cmake_minimum_required(VERSION 3.0)

include_directories(include)
file(GLOB_RECURSE src *.c *.cpp)

# add_library(simtochat ${src})