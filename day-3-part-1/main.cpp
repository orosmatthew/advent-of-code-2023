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

static int solve(const std::string& data)
{
    static std::array pow10 { 1, 10, 100 };
    const int size = line_width(data);
    const std::array<Vector2i, 8> check_offsets {
        { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } }
    };
    std::array<int, 3> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;
    int total = 0;
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const int current_idx = unsafe_pos_to_idx(size, { x, y });
            const bool is_digit = data[current_idx] >= '0' && data[current_idx] <= '9';
            if (is_digit) {
                num_buffer[num_size] = data[current_idx] - '0';
                num_size++;
            }
            if ((!is_digit || x == size - 1) && num_size != 0) {
                for (int n = 0; n < num_size; ++n) {
                    for (const auto [offset_x, offset_y] : check_offsets) {
                        const std::optional<int> idx = pos_to_idx(size, { x - n - 1 + offset_x, y + offset_y });
                        if (!idx.has_value()) {
                            continue;
                        }
                        if (data[*idx] != '.' && !(data[*idx] >= '0' && data[*idx] <= '9')) {
                            goto has_symbol;
                        }
                    }
                }
                num_size = 0;
                continue;
            has_symbol:
                for (int n = 0; n < num_size; ++n) {
                    total += pow10[num_size - n - 1] * num_buffer[n];
                }
                num_size = 0;
            }
        }
        num_size = 0;
    }
    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-3-part-1/input.txt");

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
