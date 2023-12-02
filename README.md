# Advent of Code 2023 in C++

My advent of code solutions. The goal is to have the fasted execution speed so benchmarking code is added to each day.

## Build Instructions

Clang is my preferred compiler ;)

```bash
git clone https://github.com/orosmatthew/advent-of-code-2023/tree/master
cd advent-of-code-2023
# Add -DBENCHMARK for benchmarking
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release 
cmake --build build
```