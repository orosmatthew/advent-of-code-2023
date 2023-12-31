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
add_executable(day-4-part-2 day-4-part-2/main.cpp)

add_executable(day-5-part-1 day-5-part-1/main.cpp)
add_executable(day-5-part-2 day-5-part-2/main.cpp)

add_executable(day-6-part-1 day-6-part-1/main.cpp)
add_executable(day-6-part-2 day-6-part-2/main.cpp)

add_executable(day-7-part-1 day-7-part-1/main.cpp)
add_executable(day-7-part-2 day-7-part-2/main.cpp)

add_executable(day-8-part-1 day-8-part-1/main.cpp)
add_executable(day-8-part-2 day-8-part-2/main.cpp)

add_executable(day-9-part-1 day-9-part-1/main.cpp)
add_executable(day-9-part-2 day-9-part-2/main.cpp)

add_executable(day-10-part-1 day-10-part-1/main.cpp)
add_executable(day-10-part-2 day-10-part-2/main.cpp)

add_executable(day-11-part-1 day-11-part-1/main.cpp)
add_executable(day-11-part-2 day-11-part-2/main.cpp)
