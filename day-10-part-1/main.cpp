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

static Vector2i find_start_pos(const std::string& data, const int size)
{
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if (data[unsafe_pos_to_idx(size, { x, y })] == 'S') {
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

static Direction walk_pipe(const char c, const Direction from)
{
    switch (c) {
    case '|':
        if (from == Direction::north) {
            return Direction::south;
        }
        return Direction::north;
    case '-':
        if (from == Direction::east) {
            return Direction::west;
        }
        return Direction::east;
    case 'L':
        if (from == Direction::north) {
            return Direction::east;
        }
        return Direction::north;
    case 'J':
        if (from == Direction::north) {
            return Direction::west;
        }
        return Direction::north;
    case '7':
        if (from == Direction::south) {
            return Direction::west;
        }
        return Direction::south;
    case 'F':
        if (from == Direction::east) {
            return Direction::south;
        }
        return Direction::east;
    default:
        throw std::runtime_error("Invalid pipe");
    }
}

static int solve(const std::string& data)
{
    int size = 0;
    while (data[size] != '\n') {
        size++;
    }

    const auto [start_x, start_y] = find_start_pos(data, size);
    constexpr std::array directions = { Direction::north, Direction::east, Direction::south, Direction::west };
    Direction current_from {};
    Vector2i current_pos {};
    for (const Direction direction : directions) {
        const auto [offset_x, offset_y] = direction_offset(direction);
        std::optional<int> pipe_idx = pos_to_idx(size, { start_x + offset_x, start_y + offset_y });
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

    int count = 1;
    while (data[unsafe_pos_to_idx(size, current_pos)] != 'S') {
        const Direction to = walk_pipe(data[unsafe_pos_to_idx(size, current_pos)], current_from);
        const auto [offset_x, offset_y] = direction_offset(to);
        current_pos = { current_pos.x + offset_x, current_pos.y + offset_y };
        current_from = opposite(to);
        count++;
    }

    return count / 2;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-10-part-1/input.txt");

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
