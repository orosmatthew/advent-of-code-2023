#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

static std::string read_data(const std::filesystem::path& path)
{
    std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

static int parse_num(uint32_t& result, const std::string& data, const int start_idx)
{
    static std::array<int, 10> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;
    int idx = start_idx;
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
    return idx;
}

static int parse_and_apply_map(
    std::array<uint32_t, 20>& seeds, const int seeds_size, const std::string& data, const int start_idx)
{
    std::array<std::optional<uint32_t>, 20> new_seeds {};
    int i = start_idx - 1;
    do {
        i++;
        uint32_t dst_start;
        i = parse_num(dst_start, data, i);
        i++;
        uint32_t src_start;
        i = parse_num(src_start, data, i);
        i++;
        uint32_t length;
        i = parse_num(length, data, i);
        for (int s = 0; s < seeds_size; ++s) {
            if (seeds[s] >= src_start && seeds[s] < src_start + length) {
                new_seeds[s] = seeds[s] + (dst_start - src_start);
            }
        }
    } while (data[i] == '\n' && i + 1 < data.size() && data[i + 1] != '\n');
    for (int s = 0; s < seeds_size; ++s) {
        if (new_seeds[s].has_value()) {
            seeds[s] = *new_seeds[s];
        }
    }
    return i;
}

static uint32_t solve(const std::string& data)
{
    std::array<uint32_t, 20> seeds; // NOLINT(*-pro-type-member-init)
    int seeds_size = 0;
    int i = 7;
    while (data[i] != '\n') {
        i = parse_num(seeds[seeds_size++], data, i);
        i++;
    }
    i += 19; // for "seed-to-soil map:"
    i = parse_and_apply_map(seeds, seeds_size, data, i);
    i += 26; // for "soil-to-fertilizer map:"
    i = parse_and_apply_map(seeds, seeds_size, data, i);
    i += 27; // for "fertilizer-to-water map:"
    i = parse_and_apply_map(seeds, seeds_size, data, i);
    i += 23; //  for "water-to-light map:"
    i = parse_and_apply_map(seeds, seeds_size, data, i);
    i += 29; //  for "light-to-temperature map:"
    i = parse_and_apply_map(seeds, seeds_size, data, i);
    i += 31; //  for "temperature-to-humidity map:"
    i = parse_and_apply_map(seeds, seeds_size, data, i);
    i += 28; //  for "humidity-to-location map:"
    parse_and_apply_map(seeds, seeds_size, data, i);

    uint32_t min_loc = seeds[0];
    for (int s = 1; s < seeds_size; ++s) {
        if (seeds[s] < min_loc) {
            min_loc = seeds[s];
        }
    }
    return min_loc;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-5-part-1/input.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 100000;
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
