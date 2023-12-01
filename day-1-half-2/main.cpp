#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

std::string read_data(const std::filesystem::path& path)
{
    std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

struct DigitStr {
    const char* str;
    int length;
    char value;
};

int main()
{
    const std::string data = read_data("../day-1-half-2/input.txt");

    std::vector<double> times;
    for (int n_run = 0; n_run < 10000; ++n_run) {
        auto start = std::chrono::high_resolution_clock::now();

        int total = 0;

        std::array<DigitStr, 9> digit_strs
            = { { { "one", 3, '1' },
                  { "two", 3, '2' },
                  { "three", 5, '3' },
                  { "four", 4, '4' },
                  { "five", 4, '5' },
                  { "six", 3, '6' },
                  { "seven", 5, '7' },
                  { "eight", 5, '8' },
                  { "nine", 4, '9' } } };

        std::optional<char> first;
        std::optional<char> last;
        for (const char& c : data) {
            if (std::isdigit(c)) {
                if (!first.has_value()) {
                    first = c;
                }
                last = c;
            }
            else if (std::isalpha(c)) {
                for (const auto [str, length, value] : digit_strs) {
                    if (std::strncmp(&c, str, length) == 0) {
                        if (!first.has_value()) {
                            first = value;
                        }
                        last = value;
                        break;
                    }
                }
            }
            else if (c == '\n') {
                assert(first.has_value() && last.has_value() && "There must be at least one number");
                const char num_str[3] = { first.value(), last.value(), '\0' };
                total += std::stoi(num_str);
                first.reset();
                last.reset();
            }
        }
        // std::cout << total << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        times.push_back(std::chrono::duration<double, std::nano>(end - start).count());
    }
    double total = 0.0;
    for (const double time : times) {
        total += time;
    }
    std::cout << "Average ns: " << total / static_cast<double>(times.size()) << std::endl;
}
