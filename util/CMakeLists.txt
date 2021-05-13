cmake_minimum_required(VERSION 3.0)

link_libraries(libmysqlclient.so)

include_directories(include)
file(GLOB_RECURSE src *.c *.cpp)
add_library(util ${src})