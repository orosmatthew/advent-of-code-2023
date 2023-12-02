#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <cstring>

static std::string read_data(const std::filesystem::path& path)
{
    std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

static int parse_int(int& result, const std::string& data, const int start_idx)
{
    static std::array pow10 { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
    std::array<int, 10> buffer; // NOLINT(*-pro-type-member-init)
    int idx = start_idx;
    while (data[idx] >= '0' && data[idx] <= '9') {
        buffer[idx - start_idx] = data[idx] - '0';
        idx++;
    }
    result = 0;
    for (int i = 0; i < idx - start_idx; ++i) {
        result += pow10[idx - start_idx - i - 1] * buffer[i];
    }
    return idx;
}

static int skip_int(const std::string& data, const int start_idx)
{
    int idx = start_idx;
    while (data[idx] >= '0' && data[idx] <= '9') {
        idx++;
    }
    return idx;
}

static int parse_set(std::array<std::optional<int>, 3>& result, const std::string& data, const int start_idx)
{
    static std::array<std::pair<const char*, int>, 3> color_strs = { { { "red", 3 }, { "green", 5 }, { "blue", 4 } } };
    for (int i = start_idx; i < data.size(); ++i) {
        int num;
        i = parse_int(num, data, i);
        i++; // space
        int color_idx = -1;
        for (int c = 0; c < color_strs.size(); ++c) {
            if (std::memcmp(&data[i], color_strs[c].first, color_strs[c].second) == 0) {
                color_idx = c;
                i += color_strs[c].second;
                break;
            }
        }
        assert(color_idx != -1 && "Must be a valid color");
        result[color_idx] = num;
        if (data[i] == ';') {
            i += 2;
            return i;
        }
        if (data[i] == '\n') {
            return i;
        }
        i++; // for ,
    }
    return static_cast<int>(data.size()) - 1;
}

static int solve(const std::string& data)
{
    int power_sum = 0;
    for (int i = 5; i < data.size(); ++i) {
        std::array color_mins = { 0, 0, 0 };
        i = skip_int(data, i);
        i += 2; // for colon and space
        while (data[i] != '\n') {
            std::array<std::optional<int>, 3> set;
            i = parse_set(set, data, i);
            for (int c = 0; c < set.size(); ++c) {
                if (set[c].has_value() && *set[c] > color_mins[c]) {
                    color_mins[c] = *set[c];
                }
            }
        }
        power_sum += color_mins[0] * color_mins[1] * color_mins[2];
        i += 5; // for "Game "
    }
    return power_sum;
}

#define BENCHMARK

int main()
{
    const std::string data = read_data("./day-2-part-2/input.txt");

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
