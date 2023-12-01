cmake_minimum_required(VERSION 3.27)
project(advent_of_code_2023)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -stdlib=libc++ -lc++abi")
    endif ()
endif ()

add_executable(day-1-half-1 day-1-half-1/main.cpp)
add_executable(day-1-half-2 day-1-half-2/main.cpp)
