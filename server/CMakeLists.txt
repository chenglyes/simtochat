cmake_minimum_required(VERSION 3.0)

include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/lib/socklib/include)
file(GLOB_RECURSE SRC src/*.c src/*.cpp)

#link_directories(${PROJECT_SOURCE_DIR}/lib/socklib/lib)

link_libraries(socklib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_executable(server ${SRC})
