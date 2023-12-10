#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

static std::string read_data(const std::filesystem::path& path)
{
    const std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

static int solve(const std::string& data)
{
    int total = 0;

    for (int i = 0; i < data.size(); ++i) {
        int first = -1;
        int forward_idx = i;
        for (; forward_idx < data.size(); ++forward_idx) {
            if (data[forward_idx] >= '0' && data[forward_idx] <= '9') {
                first = data[forward_idx] - '0';
                break;
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
    const std::string data = read_data("./day-1-part-1/input.txt");

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
