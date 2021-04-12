cmake_minimum_required(VERSION 3.0)

include_directories(include)
file(GLOB_RECURSE SRC src/*.c src/*.cpp)

add_library(socklib STATIC ${SRC})
