#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

std::string read_data(const std::filesystem::path& path)
{
    std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

int solve(const std::string& data)
{
    int total = 0;
    std::optional<char> first;
    std::optional<char> last;
    for (const char c : data) {
        if (std::isdigit(c)) {
            if (!first.has_value()) {
                first = c;
            }
            last = c;
        }
        else if (c == '\n') {
            assert(first.has_value() && last.has_value() && "There must be at least one number");
            std::array num_str = { first.value(), last.value(), '\0' };
            total += std::stoi(num_str.data());
            first.reset();
            last.reset();
        }
    }

    return total;
}

int main()
{
    const std::string data = read_data("./day-1-part-1/input.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 10000;
    double time_running_total = 0.0;

    for (int n_run = 0; n_run < n_runs; ++n_run) {
        auto start = std::chrono::high_resolution_clock::now();
        solve(data);
        auto end = std::chrono::high_resolution_clock::now();
        time_running_total += std::chrono::duration<double, std::nano>(end - start).count();
    }

    std::cout << "Average ns: " << time_running_total / n_runs << std::endl;
#else
    std::cout << solve(data) << std::endl;
#endif
}