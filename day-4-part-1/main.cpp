#include <array>
#include <bitset>
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
    static std::array<int, 3> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;

    int idx = start_idx;
    while (data[idx] == ' ' || data[idx] == '|') {
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
    default:
        throw std::runtime_error("Unreachable");
    }
    return idx;
}

static int solve(const std::string& data)
{
    int total = 0;
    for (int idx = 0; idx < data.size(); ++idx) {
        std::bitset<128> winning_nums { 0 };
        // move past "Card X"
        while (data[idx] != ':') {
            idx++;
        }
        idx++; // for ":"
        while (data[idx + 1] != '|') {
            int winning_num;
            idx = parse_num(winning_num, data, idx);
            winning_nums[winning_num] = true;
        }

        int score = 0;
        while (data[idx] != '\n') {
            int num;
            idx = parse_num(num, data, idx);
            if (winning_nums[num] == true) {
                if (score == 0) {
                    score = 1;
                }
                else {
                    score *= 2;
                }
            }
        }
        total += score;
    }
    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-4-part-1/input.txt");

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
