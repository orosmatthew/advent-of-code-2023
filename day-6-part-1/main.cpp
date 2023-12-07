#include <array>
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

static int parse_num(int& result, const std::string& data, const int start_idx)
{
    static std::array<int, 4> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;

    int idx = start_idx;
    while (!(data[idx] >= '0' && data[idx] <= '9')) {
        idx++;
    }
    while (data[idx] >= '0' && data[idx] <= '9') {
        num_buffer[num_size] = data[idx] - '0';
        num_size++;
        idx++;
    }
    switch (num_size) {
    case 1:
        result = num_buffer[0];
        break;
    case 2:
        result = 10 * num_buffer[0] + num_buffer[1];
        break;
    case 3:
        result = 100 * num_buffer[0] + 10 * num_buffer[1] + num_buffer[2];
        break;
    case 4:
        result = 1000 * num_buffer[0] + 100 * num_buffer[1] + 10 * num_buffer[2] + num_buffer[3];
        break;
    default:
        throw std::runtime_error("Number too large");
    }
    return idx;
}

static int solve(const std::string& data)
{
    std::array<int, 4> times; // NOLINT(*-pro-type-member-init)
    int times_size = 0;

    int i = 0;
    while (data[i] != '\n') {
        i = parse_num(times[times_size++], data, i);
    }
    i++;
    int total = 0;
    for (int time_idx = 0; time_idx < times_size; ++time_idx) {
        int best_dist;
        const int& time = times[time_idx];
        i = parse_num(best_dist, data, i);
        const int max = std::floor((-time - std::sqrt(time * time - 4 * best_dist)) / -2);
        const int min = std::ceil((-time + std::sqrt(time * time - 4 * best_dist)) / -2);
        if (total == 0) {
            total = max - min + 1;
        }
        else {
            total *= max - min + 1;
        }
    }

    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-6-part-1/input.txt");

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
