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

static bool is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

static int parse_num(int& result, const std::string& data, const int start_idx)
{
    std::array<int, 3> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;

    int idx = start_idx;
    // move past whitespace
    while (!is_digit(data[idx])) {
        idx++;
    }
    while (is_digit(data[idx])) {
        num_buffer[num_size] = data[idx] - '0';
        num_size++;
        idx++;
    }
    static std::array pow10 { 1, 10, 100 };
    result = 0;
    for (int n = 0; n < num_size; ++n) {
        result += pow10[num_size - n - 1] * num_buffer[n];
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
        std::array<int, 10> winning_nums; // NOLINT(*-pro-type-member-init)
        int winning_nums_size = 0;
        while (data[idx + 1] != '|') {
            idx = parse_num(winning_nums[winning_nums_size++], data, idx);
        }

        int card_matches = 0;
        while (data[idx] != '\n') {
            int num;
            idx = parse_num(num, data, idx);
            for (int i = 0; i < winning_nums_size; ++i) {
                if (num == winning_nums[i]) {
                    card_matches++;
                    break;
                }
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

#define BENCHMARK

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
