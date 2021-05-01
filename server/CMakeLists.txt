cmake_minimum_required(VERSION 3.0)

include_directories(${PROJECT_SOURCE_DIR}/simtochat/include ${PROJECT_SOURCE_DIR}/util/include)
link_libraries(util)

include_directories(include)
file(GLOB_RECURSE src *.c *.cpp)
add_executable(server ${src})