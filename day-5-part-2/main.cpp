#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

static std::string read_data(const std::filesystem::path& path)
{
    std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

static int parse_num(uint32_t& result, const std::string& data, const int start_idx)
{
    static std::array<int, 10> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;
    int idx = start_idx;
    while (data[idx] >= '0' && data[idx] <= '9') {
        num_buffer[num_size] = data[idx] - '0';
        num_size++;
        idx++;
    }
    assert(num_size <= 10 && "Number too big");
    result = 0;
    static std::array pow10 { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
    for (int n = 0; n < num_size; ++n) {
        result += pow10[num_size - n - 1] * num_buffer[n];
    }
    return idx;
}

struct Range {
    uint32_t start;
    uint32_t end;
};

struct MapRangeResult {
    Range moved {};
    std::optional<Range> extra_left;
    std::optional<Range> extra_right;
};

static std::optional<MapRangeResult> map_range(const Range& range, const Range& from, const uint32_t offset)
{
    if (range.end < from.start || range.start > from.end) {
        return {};
    }
    MapRangeResult result;
    result.moved = range;
    if (range.start < from.start) {
        result.extra_left = Range { .start = range.start, .end = from.start - 1 };
        result.moved.start = from.start;
    }
    if (range.end > from.end) {
        result.extra_right = Range { .start = from.end + 1, .end = range.end };
        result.moved.end = from.end;
    }
    result.moved.start += offset;
    result.moved.end += offset;
    return result;
}

[[nodiscard]] static std::vector<Range> consolidate_ranges(std::vector<Range>& ranges)
{
    assert(!ranges.empty());

    std::ranges::remove_if(ranges, [](const Range& range) { return range.start > range.end; });

    std::ranges::sort(ranges, [](const Range& a, const Range& b) { return a.start < b.start; });

    std::vector<Range> consolidated;
    consolidated.push_back(ranges[0]);

    for (const Range range : ranges) {
        if (auto& [start, end] = consolidated.back(); range.start <= end) {
            end = range.end;
        }
        else {
            consolidated.push_back(range);
        }
    }
    return consolidated;
}

static std::pair<int, std::vector<Range>> parse_and_apply_map(
    const std::vector<Range>& seed_ranges, const std::string& data, const int start_idx)
{
    std::vector<Range> moved_ranges;
    int i = start_idx - 1;
    do {
        i++;
        uint32_t dst_start;
        i = parse_num(dst_start, data, i);
        i++;
        uint32_t src_start;
        i = parse_num(src_start, data, i);
        i++;
        uint32_t length;
        i = parse_num(length, data, i);
        for (const Range& range : seed_ranges) {
            std::optional<MapRangeResult> result
                = map_range(range, { src_start, src_start + length - 1 }, dst_start - src_start);
            if (!result.has_value()) {
                continue;
            }
            moved_ranges.push_back(result->moved);
        }
    } while (data[i] == '\n' && i + 1 < data.size() && data[i + 1] != '\n');

    // TODO: subtract ranges

    return { i, new_ranges };
}

static uint32_t solve(const std::string& data)
{
    std::vector<Range> seed_ranges;
    int i = 7;
    while (data[i] != '\n') {
        uint32_t start;
        i = parse_num(start, data, i);
        i++;
        uint32_t length;
        i = parse_num(length, data, i);
        i++;
        seed_ranges.push_back(Range { start, start + length - 1 });
    }

    // seed_ranges = consolidate_ranges(seed_ranges);

    for (constexpr std::array offsets { 19, 26, 27, 23, 28, 31, 28 }; const int offset : offsets) {
        i += offset;
        auto [new_idx, new_ranges] = parse_and_apply_map(seed_ranges, data, i);
        i = new_idx;
        seed_ranges = std::move(new_ranges);
    }

    return seed_ranges[0].start;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-5-part-2/sample.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 100000;
    double time_running_total = 0.0;

    for (int n_run = 0; n_run < n_runs; ++n_run) {
        auto start = std::chrono::high_resolution_clock::now();
        // ReSharper disable once CppDFAUnusedValue
        // ReSharper disable once CppDFAUnreadVariable
        // ReSharper disable once CppDeclaratorNeverUsed
        volatile uint32_t result = solve(data);
        auto end = std::chrono::high_resolution_clock::now();
        time_running_total += std::chrono::duration<double, std::nano>(end - start).count();
    }

    std::cout << "Average ns: " << time_running_total / n_runs << std::endl;
#else
    std::cout << solve(data) << std::endl;
#endif
}
