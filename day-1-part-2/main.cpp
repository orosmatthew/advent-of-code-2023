#include <array>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

static std::string read_data(const std::filesystem::path& path)
{
    std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

struct DigitStr {
    const char* str;
    int length;
    int value;
};

static int solve(const std::string& data)
{
    int total = 0;

    std::array<DigitStr, 9> digit_strs
        = { { { "one", 3, 1 },
              { "two", 3, 2 },
              { "three", 5, 3 },
              { "four", 4, 4 },
              { "five", 4, 5 },
              { "six", 3, 6 },
              { "seven", 5, 7 },
              { "eight", 5, 8 },
              { "nine", 4, 9 } } };

    for (int i = 0; i < data.size(); ++i) {
        int first = -1;
        int forward_idx = i;
        for (; forward_idx < data.size(); ++forward_idx) {
            if (data[forward_idx] >= '0' && data[forward_idx] <= '9') {
                first = data[forward_idx] - '0';
                break;
            }
            if (data[forward_idx] >= 'a' && data[forward_idx] <= 'z') {
                for (const auto [str, length, value] : digit_strs) {
                    if (std::memcmp(&data[forward_idx], str, length) == 0) {
                        first = value;
                        goto first_done;
                    }
                }
            }
        }
    first_done:
        int reverse_idx = forward_idx;
        int next_idx = static_cast<int>(data.size()) - 1;
        for (; reverse_idx < data.size(); ++reverse_idx) {
            if (data[reverse_idx] == '\n') {
                next_idx = reverse_idx;
                reverse_idx--;
                break;
            }
        }
        int last = -1;
        for (; reverse_idx >= forward_idx; --reverse_idx) {
            if (data[reverse_idx] >= '0' && data[reverse_idx] <= '9') {
                last = data[reverse_idx] - '0';
                break;
            }
            if (data[reverse_idx] >= 'a' && data[reverse_idx] <= 'z') {
                for (const auto [str, length, value] : digit_strs) {
                    if (std::memcmp(&data[reverse_idx], str, length) == 0) {
                        last = value;
                        goto last_done;
                    }
                }
            }
        }
    last_done:
        assert(first != -1 && last != -1 && "There must be at least one number");
        total += 10 * first + last;
        i = next_idx;
    }

    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-1-part-2/input.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 100000;
    double time_running_total = 0.0;

    for (int n_run = 0; n_run < n_runs; ++n_run) {
        auto start = std::chrono::high_resolution_clock::now();
        // ReSharper disable once CppDFAUnusedValue
        // ReSharper disable once CppDFAUnreadVariable
        // ReSharper disable once CppDeclaratorNeverUsed
        volatile int result = solve(data);
        auto end = std::chrono::high_resolution_clock::now();
        time_running_total += std::chrono::duration<double, std::nano>(end - start).count();
    }

    std::cout << "Average ns: " << time_running_total / n_runs << std::endl;
#else
    std::cout << solve(data) << std::endl;
#endif
}
