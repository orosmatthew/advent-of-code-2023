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

int main()
{
    const std::string data = read_data("../day-1-part-1/input.txt");
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

    std::cout << total << std::endl;
}
