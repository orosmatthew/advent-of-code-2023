#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

static std::string read_data(const std::filesystem::path& path)
{
    const std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

struct Vector2i {
    int x;
    int y;
};

static int unsafe_pos_to_idx(const int width, const Vector2i pos)
{
    return pos.y * (width + 1) + pos.x;
}

static int solve(const std::string& data)
{
    int width = 0;
    while (data[width] != '\n') {
        width++;
    }
    const int height = static_cast<int>(data.size()) / (width + 1);

    std::vector<Vector2i> galaxies;

    int y_offset = 0;
    for (int y = 0; y < height; ++y) {
        bool galaxy_found = false;
        for (int x = 0; x < height; ++x) {
            if (data[unsafe_pos_to_idx(width, { x, y })] == '#') {
                galaxies.push_back({ x, y + y_offset });
                galaxy_found = true;
            }
        }
        y_offset += galaxy_found ? 0 : 1;
    }

    int x_offset = 0;
    for (int x = 0; x < width; ++x) {
        bool galaxy_found = false;
        for (int y = 0; y < height; ++y) {
            if (data[unsafe_pos_to_idx(width, { x, y })] == '#') {
                galaxy_found = true;
                break;
            }
        }
        if (!galaxy_found) {
            for (auto& [gal_x, _] : galaxies) {
                if (gal_x > x + x_offset) {
                    gal_x++;
                }
            }
        }
        x_offset += galaxy_found ? 0 : 1;
    }

    int cost = 0;
    for (int i = 0; i < galaxies.size() - 1; ++i) {
        for (int j = i + 1; j < galaxies.size(); ++j) {
            const auto& [a_x, a_y] = galaxies[i];
            const auto& [b_x, b_y] = galaxies[j];
            cost += std::abs(a_x - b_x) + std::abs(a_y - b_y);
        }
    }
    return cost;
}

#define BENCHMARK

int main()
{
    const std::string data = read_data("./day-11-part-1/input.txt");

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
