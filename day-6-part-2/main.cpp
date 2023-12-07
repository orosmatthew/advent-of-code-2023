#include <array>
#include <cassert>
#include <cmath>
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

static int parse_num(int64_t& result, const std::string& data, const int start_idx)
{
    static std::array<int, 15> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;

    int idx = start_idx;
    while (!(data[idx] >= '0' && data[idx] <= '9')) {
        idx++;
    }
    while ((data[idx] >= '0' && data[idx] <= '9') || data[idx] == ' ') {
        if (data[idx] == ' ') {
            idx++;
            continue;
        }
        num_buffer[num_size] = data[idx] - '0';
        num_size++;
        idx++;
    }
    assert(num_size <= 15 && "Number too big");
    result = 0;
    static std::array<int64_t, 15> pow10 { 1,           10,           100,           1000,           10000,
                                           100000,      1000000,      10000000,      100000000,      1000000000,
                                           10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000 };
    for (int n = 0; n < num_size; ++n) {
        result += pow10[num_size - n - 1] * num_buffer[n];
    }
    return idx;
}

static int64_t solve(const std::string& data)
{
    int i = 0;
    int64_t time;
    i = parse_num(time, data, i);
    i++;
    int64_t best_dist;
    parse_num(best_dist, data, i);

    const int64_t max = std::floor((-static_cast<double>(time) - std::sqrt(time * time - 4 * best_dist)) / -2);
    const int64_t min = std::ceil((-static_cast<double>(time) + std::sqrt(time * time - 4 * best_dist)) / -2);

    return max - min + 1;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-6-part-2/input.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 1000000;
    double time_running_total = 0.0;

    for (int n_run = 0; n_run < n_runs; ++n_run) {
        auto start = std::chrono::high_resolution_clock::now();
        // ReSharper disable once CppDFAUnusedValue
        // ReSharper disable once CppDFAUnreadVariable
        // ReSharper disable once CppDeclaratorNeverUsed
        volatile uint32_t result = solve(data);
        auto end = std::chrono::high_resolution_clock::now();
        time_running_total += std::chrono::duration<double, std::nano>(end - start).count();
    }

    std::cout << "Average ns: " << time_running_total / n_runs << std::endl;
#else
    std::cout << solve(data) << std::endl;
#endif
}
