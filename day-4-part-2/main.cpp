#include <array>
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

static int matches(std::array<int, 256>& matches_arr, const std::string& data)
{
    int n_matches = 0;
    for (int idx = 0; idx < data.size(); ++idx) {
        // move past "Card X:"
        while (data[idx] != ':') {
            idx++;
        }
        idx++; // for ":"
        std::bitset<128> winning_nums { 0 };
        while (data[idx + 1] != '|') {
            int winning_num;
            idx = parse_num(winning_num, data, idx);
            winning_nums[winning_num] = true;
        }

        int card_matches = 0;
        while (data[idx] != '\n') {
            int num;
            idx = parse_num(num, data, idx);
            if (winning_nums[num] == true) {
                card_matches++;
            }
        }
        matches_arr[n_matches++] = card_matches;
    }
    return n_matches;
}

static int count_cards(const std::array<int, 256>& matches_arr, const int idx) // NOLINT(*-no-recursion)
{
    static std::array<std::optional<int>, 256> count_cards_cache {};
    if (const std::optional<int> cached_value = count_cards_cache[idx]) {
        return *cached_value;
    }
    int total = 1;
    for (int i = idx + 1; i < idx + matches_arr[idx] + 1; ++i) {
        total += count_cards(matches_arr, i);
    }
    count_cards_cache[idx] = total;
    return total;
}

static int solve(const std::string& data)
{
    std::array<int, 256> matches_arr; // NOLINT(*-pro-type-member-init)
    const int matches_arr_size = matches(matches_arr, data);
    int total = 0;
    for (int i = 0; i < matches_arr_size; ++i) {
        total += count_cards(matches_arr, i);
    }
    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-4-part-2/input.txt");

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
