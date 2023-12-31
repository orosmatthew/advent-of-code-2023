# Advent of Code 2023 in C++

My advent of code solutions. The goal is to have the fastest execution speed so benchmarking code is added to each day.

## Benchmarks

All tests are performed on latest Windows 11 23H2 with Clang 17 using a 12600K CPU. The benchmarks do not include
loading the input file into memory, only solving the problem.

|    Problem    | Average Nanoseconds | Iterations |
| :-----------: | :-----------------: | :--------: |
| Day 1 Part 1  |       16,865        |  100,000   |
| Day 1 Part 2  |       21,222        |  100,000   |
| Day 2 Part 1  |        3,271        |  100,000   |
| Day 2 Part 2  |        3,601        |  100,000   |
| Day 3 Part 1  |       27,312        |  100,000   |
| Day 3 Part 2  |       12,473        |  100,000   |
| Day 4 Part 1  |       23,489        |  100,000   |
| Day 4 Part 2  |       22,485        |  100,000   |
| Day 5 Part 1  |        7,729        |  100,000   |
| Day 5 Part 2  |       15,268        |  100,000   |
| Day 6 Part 1  |         92          | 1,000,000  |
| Day 6 Part 2  |         71          | 1,000,000  |
| Day 7 Part 1  |       105,161       |   10,000   |
| Day 7 Part 2  |       111,670       |   10,000   |
| Day 8 Part 1  |       51,663        |  100,000   |
| Day 8 Part 2  |       301,582       |   10,000   |
| Day 9 Part 1  |       42,537        |  100,000   |
| Day 9 Part 2  |       42,210        |  100,000   |
| Day 10 Part 1 |       58,608        |  100,000   |
| Day 10 Part 2 |       122,842       |   10,000   |
| Day 11 Part 1 |       38,913        |  100,000   |
| Day 11 Part 2 |       65,044        |  100,000   |

## Build Instructions

Clang is my preferred compiler ;)

```bash
git clone https://github.com/orosmatthew/advent-of-code-2023/tree/master
cd advent-of-code-2023
# Add -DBENCHMARK=On for benchmarking
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release 
cmake --build build
```