#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

static std::string read_data(const std::filesystem::path& path)
{
    const std::fstream in_file(path);
    std::stringstream ss;
    ss << in_file.rdbuf();
    return ss.str();
}

class NodeIdent {
public:
    NodeIdent() = default;

    explicit NodeIdent(const std::array<char, 3> data)
        : m_data(data)
        , m_index(calc_index(data))
    {
    }

    bool operator==(const NodeIdent& other) const
    {
        return m_data == other.m_data;
    }

    [[nodiscard]] size_t index() const
    {
        return m_index;
    }

    [[nodiscard]] bool is_start() const
    {
        return m_data[2] == 'A';
    }

    [[nodiscard]] bool is_end() const
    {
        return m_data[2] == 'Z';
    }

private:
    static int calc_index(const std::array<char, 3>& ident)
    {
        int total = 0;
        total += ident[0] >= 'A' ? ident[0] - 'A' + 10 : ident[0] - '0';
        total += ident[1] >= 'A' ? (ident[1] - 'A' + 10) * 36 : (ident[0] - '0') * 36;
        total += ident[2] >= 'A' ? (ident[2] - 'A' + 10) * 1296 : (ident[0] - '0') * 1296;
        return total;
    }

    std::array<char, 3> m_data {};
    size_t m_index {};
};

struct Node {
    NodeIdent from;
    std::pair<NodeIdent, NodeIdent> to;
};

static std::pair<int, Node> parse_node(const std::string& data, const int start_idx)
{
    auto parse_ident = [&](NodeIdent& result, const int start) -> int {
        std::array<char, 3> d; // NOLINT(*-pro-type-member-init)
        d[0] = data[start];
        d[1] = data[start + 1];
        d[2] = data[start + 2];
        result = NodeIdent(d);
        return start + 3;
    };

    int idx = start_idx;
    NodeIdent from; // NOLINT(*-pro-type-member-init)
    idx = parse_ident(from, idx);
    idx += 4; // for " = ("
    NodeIdent left; // NOLINT(*-pro-type-member-init)
    idx = parse_ident(left, idx);
    idx += 2; // for ", "
    NodeIdent right; // NOLINT(*-pro-type-member-init)
    idx = parse_ident(right, idx);
    idx += 2; // for ")\n"
    return { idx, Node(from, std::pair(left, right)) };
}

static uint64_t solve(const std::string& data)
{
    static std::vector<char> instructions;
    instructions.clear();
    int i = 0;
    while (data[i] != '\n') {
        instructions.push_back(data[i]);
        i++;
    }
    i += 2; // for 2 newlines
    static std::vector<Node> starting_nodes;
    starting_nodes.clear();
    static std::vector<Node> nodes;
    nodes.clear();
    nodes.resize(47988);
    while (i < data.size()) {
        auto [new_i, node] = parse_node(data, i);
        i = new_i;
        if (node.from.is_start()) {
            starting_nodes.push_back(node);
        }
        nodes[node.from.index()] = std::move(node);
    }

    static std::vector<int> path_lengths;
    path_lengths.clear();
    path_lengths.reserve(starting_nodes.size());
    for (const auto& [from, to] : starting_nodes) {
        const Node* current = &nodes[from.index()];
        int offset_count = 1;
        while (!current->from.is_end()) {
            if (const char instruction = instructions[(offset_count - 1) % instructions.size()]; instruction == 'L') {
                current = &nodes[current->to.first.index()];
            }
            else {
                current = &nodes[current->to.second.index()];
            }
            offset_count++;
        }
        path_lengths.push_back(offset_count - 1);
    }

    const uint64_t lcm
        = std::accumulate(path_lengths.begin(), path_lengths.end(), 1ULL, [](const uint64_t acc, const int& length) {
              return std::lcm(acc, length);
          });

    return lcm;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-8-part-2/input.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 10000;
    double time_running_total = 0.0;

    for (int n_run = 0; n_run < n_runs; ++n_run) {
        auto start = std::chrono::high_resolution_clock::now();
        // ReSharper disable once CppDFAUnusedValue
        // ReSharper disable once CppDFAUnreadVariable
        // ReSharper disable once CppDeclaratorNeverUsed
        volatile uint64_t result = solve(data);
        auto end = std::chrono::high_resolution_clock::now();
        time_running_total += std::chrono::duration<double, std::nano>(end - start).count();
    }

    std::cout << "Average ns: " << time_running_total / n_runs << std::endl;
#else
    std::cout << solve(data) << std::endl;
#endif
}
