cmake_minimum_required(VERSION 3.27)
project(advent_of_code_2023)

set(CMAKE_CXX_STANDARD 20)

option(BENCHMARK "Enable benchmarking" OFF)
if (BENCHMARK)
    add_compile_definitions(BENCHMARK)
endif ()

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -stdlib=libc++ -lc++abi")
    endif ()
endif ()

add_executable(day-1-part-1 day-1-part-1/main.cpp)
add_executable(day-1-part-2 day-1-part-2/main.cpp)

add_executable(day-2-part-1 day-2-part-1/main.cpp)
add_executable(day-2-part-2 day-2-part-2/main.cpp)

add_executable(day-3-part-1 day-3-part-1/main.cpp)
add_executable(day-3-part-2 day-3-part-2/main.cpp)

add_executable(day-4-part-1 day-4-part-1/main.cpp)

