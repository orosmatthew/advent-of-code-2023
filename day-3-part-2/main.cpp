#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

static std::string read_data(const std::filesystem::path& path)
{
    const std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

static int line_width(const std::string& data)
{
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == '\n') {
            return i;
        }
    }
    throw std::runtime_error("There should be at least one newline");
}

struct Vector2i {
    int x;
    int y;
};

static int unsafe_pos_to_idx(const int size, const Vector2i pos)
{
    return pos.y * (size + 1) + pos.x;
}

static std::optional<int> pos_to_idx(const int size, const Vector2i pos)
{
    if (pos.x < 0 || pos.x > size - 1 || pos.y < 0 || pos.y > size - 1) {
        return {};
    }
    return unsafe_pos_to_idx(size, pos);
}

static bool is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

static std::optional<std::pair<Vector2i, Vector2i>> find_number_pair(
    const std::string& data, const int size, const Vector2i star_pos)
{
    std::optional<Vector2i> first;

    auto check_top_bottom = [&](const int offset_y) -> std::optional<std::pair<Vector2i, Vector2i>> {
        bool prev_digit = false;
        for (int offset_x = -1; offset_x < 2; ++offset_x) {
            Vector2i pos { star_pos.x + offset_x, star_pos.y + offset_y };
            std::optional<int> idx = pos_to_idx(size, pos);
            if (!idx.has_value()) {
                continue;
            }
            if (is_digit(data[*idx])) {
                if (!prev_digit) {
                    if (!first.has_value()) {
                        first = pos;
                    }
                    else {
                        return std::pair(*first, pos);
                    }
                }
                prev_digit = true;
            }
            else {
                prev_digit = false;
            }
        }
        return {};
    };

    auto check_middle = [&]() -> std::optional<std::pair<Vector2i, Vector2i>> {
        const Vector2i left { star_pos.x - 1, star_pos.y };
        if (const std::optional<int> left_idx = pos_to_idx(size, left);
            left_idx.has_value() && is_digit(data[*left_idx])) {
            if (!first.has_value()) {
                first = left;
            }
            else {
                return std::pair(*first, left);
            }
        }
        const Vector2i right { star_pos.x + 1, star_pos.y };
        if (const std::optional<int> right_idx = pos_to_idx(size, right);
            right_idx.has_value() && is_digit(data[*right_idx])) {
            if (!first.has_value()) {
                first = right;
            }
            else {
                return std::pair(*first, right);
            }
        }
        return {};
    };

    if (auto value = check_top_bottom(1)) {
        return value;
    }
    if (auto value = check_middle()) {
        return value;
    }
    if (auto value = check_top_bottom(-1)) {
        return value;
    }
    return {};
}

static int parse_num_from_rand_pos(const std::string& data, const int size, const Vector2i rand_pos)
{
    int start_x = rand_pos.x;
    if (rand_pos.x - 1 >= 0) {
        for (int x = rand_pos.x - 1; x >= 0; --x) {
            if (std::optional<int> idx = pos_to_idx(size, { x, rand_pos.y });
                !idx.has_value() || !is_digit(data[*idx])) {
                break;
            }
            start_x = x;
        }
    }

    std::array<int, 3> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;
    for (int x = start_x; x < size; ++x) {
        std::optional<int> idx = pos_to_idx(size, { x, rand_pos.y });
        if (!idx.has_value() || !is_digit(data[*idx])) {
            break;
        }
        num_buffer[num_size] = data[*idx] - '0';
        num_size++;
    }
    int total = 0;
    static std::array pow10 { 1, 10, 100 };
    for (int n = 0; n < num_size; ++n) {
        total += pow10[num_size - n - 1] * num_buffer[n];
    }
    return total;
}

static int solve(const std::string& data)
{
    const int size = line_width(data);
    int total = 0;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            if (const int current_idx = unsafe_pos_to_idx(size, { x, y }); data[current_idx] == '*') {
                if (std::optional<std::pair<Vector2i, Vector2i>> pair = find_number_pair(data, size, { x, y });
                    pair.has_value()) {
                    total += parse_num_from_rand_pos(data, size, pair->first)
                        * parse_num_from_rand_pos(data, size, pair->second);
                }
            }
        }
    }
    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-3-part-2/input.txt");

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
