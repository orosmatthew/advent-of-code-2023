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

static int parse_num(int& result, const std::string& data, const int start_idx)
{
    static std::array<int, 10> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;

    int idx = start_idx;
    bool negative = false;
    if (data[idx] == '-') {
        negative = true;
        idx++;
    }
    while (data[idx] >= '0' && data[idx] <= '9') {
        num_buffer[num_size] = data[idx] - '0';
        num_size++;
        idx++;
    }
    assert(num_size <= 10 && "Number too big");
    result = 0;
    static std::array pow10 { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
    for (int n = 0; n < num_size; ++n) {
        result += pow10[num_size - n - 1] * num_buffer[n];
    }
    if (negative) {
        result *= -1;
    }
    return idx;
}

static int extrapolate(const std::array<int, 64>& data, const int size) // NOLINT(*-no-recursion)
{
    std::array<int, 64> next; // NOLINT(*-pro-type-member-init)
    int next_size = 0;
    bool all_zero = true;
    for (int i = 1; i < size; ++i) {
        const int diff = data[i] - data[i - 1];
        if (diff != 0) {
            all_zero = false;
        }
        next[next_size++] = diff;
    }
    if (all_zero) {
        return data[size - 1];
    }
    return data[size - 1] + extrapolate(next, next_size);
}

static int solve(const std::string& data)
{

    std::array<int, 64> nums; // NOLINT(*-pro-type-member-init)
    int nums_size = 0;
    int i = 0;
    int total = 0;
    while (i < data.size()) {
        i = parse_num(nums[nums_size++], data, i);
        if (data[i] == '\n') {
            total += extrapolate(nums, nums_size);
            nums_size = 0;
        }
        i++;
    }
    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-9-part-1/input.txt");

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
