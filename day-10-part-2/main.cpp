#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
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

static std::optional<int> pos_to_idx(const int width, const int height, const Vector2i pos)
{
    if (pos.x < 0 || pos.x > width - 1 || pos.y < 0 || pos.y > height - 1) {
        return {};
    }
    return unsafe_pos_to_idx(width, pos);
}

static Vector2i find_start_pos(const std::string& data, const int width, const int height)
{
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (data[unsafe_pos_to_idx(width, { x, y })] == 'S') {
                return { x, y };
            }
        }
    }
    throw std::runtime_error("No start found");
}

enum class Direction { north, east, south, west };

static Direction opposite(const Direction d)
{
    switch (d) {
    case Direction::north:
        return Direction::south;
    case Direction::east:
        return Direction::west;
    case Direction::south:
        return Direction::north;
    case Direction::west:
        return Direction::east;
    }
    throw std::runtime_error("Unreachable");
}

static Vector2i direction_offset(const Direction d)
{
    switch (d) {
    case Direction::north:
        return { 0, -1 };
    case Direction::east:
        return { 1, 0 };
    case Direction::south:
        return { 0, 1 };
    case Direction::west:
        return { -1, 0 };
    }
    throw std::runtime_error("Unreachable");
}

static bool is_pipe(const char c)
{
    switch (c) {
    case '|':
    case '-':
    case 'L':
    case 'J':
    case '7':
    case 'F':
        return true;
    default:
        return false;
    }
}

static std::pair<Direction, Direction> pipe_directions(const char c)
{
    switch (c) {
    case '|':
        return { Direction::north, Direction::south };
    case '-':
        return { Direction::east, Direction::west };
    case 'L':
        return { Direction::north, Direction::east };
    case 'J':
        return { Direction::north, Direction::west };
    case '7':
        return { Direction::south, Direction::west };
    case 'F':
        return { Direction::east, Direction::south };
    default:
        throw std::runtime_error("Invalid pipe");
    }
}

enum class Side { left, right };

static std::pair<Direction, std::optional<Side>> walk_pipe(const char c, const Direction from)
{
    switch (c) {
    case '|':
        if (from == Direction::north) {
            return { Direction::south, {} };
        }
        return { Direction::north, {} };
    case '-':
        if (from == Direction::east) {
            return { Direction::west, {} };
        }
        return { Direction::east, {} };
    case 'L':
        if (from == Direction::north) {
            return { Direction::east, Side::left };
        }
        return { Direction::north, Side::right };
    case 'J':
        if (from == Direction::north) {
            return { Direction::west, Side::right };
        }
        return { Direction::north, Side::left };
    case '7':
        if (from == Direction::south) {
            return { Direction::west, Side::left };
        }
        return { Direction::south, Side::right };
    case 'F':
        if (from == Direction::east) {
            return { Direction::south, Side::left };
        }
        return { Direction::east, Side::right };
    default:
        throw std::runtime_error("Invalid pipe");
    }
}

static std::optional<bool> scanline_state(const char pipe, const Direction from)
{
    switch (pipe) {
    case '|':
        if (from == Direction::north) {
            return false;
        }
        return true;
    case '-':
        return {};
    case 'L':
    case 'J':
        if (from == Direction::north) {
            return false;
        }
        return true;
    case '7':
        if (from == Direction::south) {
            return true;
        }
        return false;
    case 'F':
        if (from == Direction::east) {
            return false;
        }
        return true;
    default:
        return {};
    }
}

static int solve(const std::string& data)
{
    int width = 0;
    while (data[width] != '\n') {
        width++;
    }
    const int height = static_cast<int>(data.size()) / (width + 1);

    static std::vector<char> map;
    map.clear();
    map.assign((width + 1) * height, ' ');

    const auto [start_x, start_y] = find_start_pos(data, width, height);
    constexpr std::array directions = { Direction::north, Direction::east, Direction::south, Direction::west };
    Direction current_from {};
    Vector2i current_pos {};
    for (const Direction direction : directions) {
        const auto [offset_x, offset_y] = direction_offset(direction);
        std::optional<int> pipe_idx = pos_to_idx(width, height, { start_x + offset_x, start_y + offset_y });
        if (!pipe_idx.has_value()) {
            continue;
        }
        if (!is_pipe(data[*pipe_idx])) {
            continue;
        }
        if (const auto [pipe_first, pipe_second] = pipe_directions(data[*pipe_idx]);
            pipe_first == opposite(direction) || pipe_second == opposite(direction)) {
            current_from = opposite(direction);
            current_pos = { start_x + offset_x, start_y + offset_y };
            break;
        }
    }

    int left_turn_count = 0;
    int right_turn_count = 0;
    while (data[unsafe_pos_to_idx(width, current_pos)] != 'S') {
        if (std::optional<bool> sl_state = scanline_state(data[unsafe_pos_to_idx(width, current_pos)], current_from);
            sl_state.has_value()) {
            if (*sl_state == true) {
                map[unsafe_pos_to_idx(width, current_pos)] = 'B';
            }
            else {
                map[unsafe_pos_to_idx(width, current_pos)] = 'E';
            }
        }
        else if (is_pipe(data[unsafe_pos_to_idx(width, current_pos)])) {
            map[unsafe_pos_to_idx(width, current_pos)] = 'P';
        }

        const auto [to, side] = walk_pipe(data[unsafe_pos_to_idx(width, current_pos)], current_from);
        if (side.has_value()) {
            if (*side == Side::left) {
                left_turn_count++;
            }
            else {
                right_turn_count++;
            }
        }
        const auto [offset_x, offset_y] = direction_offset(to);
        current_pos = { current_pos.x + offset_x, current_pos.y + offset_y };
        current_from = opposite(to);
    }

    int inside_count = 0;
    bool scanning = false;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int idx = unsafe_pos_to_idx(width, { x, y });
            const char map_c = map[idx];
            const char data_c = data[idx];
            if (right_turn_count > left_turn_count) { }
            if ((right_turn_count > left_turn_count && map_c == 'B')
                || (left_turn_count > right_turn_count && map_c == 'E')) {
                scanning = true;
            }
            else if (
                (right_turn_count > left_turn_count && (map_c == 'E' || map_c == 'P'))
                || (left_turn_count > right_turn_count && (map_c == 'B' || map_c == 'P'))) {
                scanning = false;
            }
            if (map_c == ' ') {
                if (data_c != 'S') {
                    if (scanning) {
                        inside_count++;
                    }
                }
            }
        }
    }

    return inside_count;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-10-part-2/input.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 10000;
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
