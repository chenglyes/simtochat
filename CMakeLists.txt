cmake_minimum_required(VERSION 3.0)

project(SimToChat)

if (NOT EXISTS ${PROJECT_SOURCE_DIR}/bin)
    execute_process(COMMAND mkdir ${PROJECT_SOURCE_DIR}/bin)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(simtochat)
add_subdirectory(util)
add_subdirectory(server)
add_subdirectory(client)