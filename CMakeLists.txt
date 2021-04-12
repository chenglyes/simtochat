cmake_minimum_required(VERSION 3.0)

if (NOT EXISTS bin)
    execute_process(COMMAND mkdir bin)
endif()

add_subdirectory(lib/socklib)
add_subdirectory(server)
add_subdirectory(client)
