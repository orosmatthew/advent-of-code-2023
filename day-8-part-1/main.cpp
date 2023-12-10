#include <array>
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

class NodeIdent {
public:
    NodeIdent() = default;

    explicit NodeIdent(const std::array<char, 3> data)
        : m_data(data)
        , m_index(data[0] - 'A' + (data[1] - 'A') * 26 + (data[2] - 'A') * 676)
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

private:
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

static int solve(const std::string& data)
{
    std::vector<char> instructions;
    int i = 0;
    while (data[i] != '\n') {
        instructions.push_back(data[i]);
        i++;
    }
    i += 2; // for 2 newlines
    std::vector<Node> nodes(18278);
    while (i < data.size()) {
        auto [new_i, node] = parse_node(data, i);
        i = new_i;
        nodes[node.from.index()] = std::move(node);
    }

    int count = 1;
    const Node* current = &nodes[NodeIdent({ 'A', 'A', 'A' }).index()];
    const NodeIdent end { { 'Z', 'Z', 'Z' } };
    while (current->from != end) {
        if (const char instruction = instructions[(count - 1) % instructions.size()]; instruction == 'L') {
            current = &nodes[current->to.first.index()];
        }
        else {
            current = &nodes[current->to.second.index()];
        }
        count++;
    }
    return --count;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-8-part-1/input.txt");

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
