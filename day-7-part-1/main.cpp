#include <algorithm>
#include <array>
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

static int parse_num(int& result, const std::string& data, const int start_idx)
{
    static std::array<int, 4> num_buffer; // NOLINT(*-pro-type-member-init)
    int num_size = 0;

    int idx = start_idx;
    while (data[idx] >= '0' && data[idx] <= '9') {
        num_buffer[num_size] = data[idx] - '0';
        num_size++;
        idx++;
    }
    switch (num_size) {
    case 1:
        result = num_buffer[0];
        break;
    case 2:
        result = 10 * num_buffer[0] + num_buffer[1];
        break;
    case 3:
        result = 100 * num_buffer[0] + 10 * num_buffer[1] + num_buffer[2];
        break;
    case 4:
        result = 1000 * num_buffer[0] + 100 * num_buffer[1] + 10 * num_buffer[2] + num_buffer[3];
        break;
    default:
        throw std::runtime_error("Number too big");
    }
    return idx;
}

enum class HandType {
    five_of_kind = 6,
    four_of_kind = 5,
    full_house = 4,
    three_of_kind = 3,
    two_pair = 2,
    one_pair = 1,
    high_card = 0,
};

static int calc_card_index(const char card)
{
    switch (card) {
    case '2':
        return 0;
    case '3':
        return 1;
    case '4':
        return 2;
    case '5':
        return 3;
    case '6':
        return 4;
    case '7':
        return 5;
    case '8':
        return 6;
    case '9':
        return 7;
    case 'T':
        return 8;
    case 'J':
        return 9;
    case 'Q':
        return 10;
    case 'K':
        return 11;
    case 'A':
        return 12;
    default:
        throw std::runtime_error("Invalid card");
    }
}

struct Hand {
    std::array<char, 5> cards;
    int bid;
    std::optional<HandType> hand_type {};

    [[nodiscard]] HandType type()
    {
        if (hand_type.has_value()) {
            return *hand_type;
        }
        std::array<int, 13> occurrences {};
        int highest_card_index = 0;
        for (int i = 0; i < 5; ++i) {
            const int card_index = calc_card_index(cards[i]);
            if (card_index > highest_card_index) {
                highest_card_index = card_index;
            }
            occurrences[card_index]++;
        }
        bool three = false;
        int pairs = 0;
        for (int i = 0; i < 13; ++i) {
            if (occurrences[i] == 5) {
                hand_type = HandType::five_of_kind;
                return HandType::five_of_kind;
            }
            if (occurrences[i] == 4) {
                hand_type = HandType::four_of_kind;
                return HandType::four_of_kind;
            }
            if (occurrences[i] == 3) {
                three = true;
            }
            if (occurrences[i] == 2) {
                pairs++;
            }
        }
        if (three) {
            if (pairs != 0) {
                hand_type = HandType::full_house;
                return HandType::full_house;
            }
            hand_type = HandType::three_of_kind;
            return HandType::three_of_kind;
        }
        if (pairs == 2) {
            hand_type = HandType::two_pair;
            return HandType::two_pair;
        }
        if (pairs == 1) {
            hand_type = HandType::one_pair;
            return HandType::one_pair;
        }
        hand_type = HandType::high_card;
        return HandType::high_card;
    }
};

static int64_t solve(const std::string& data)
{
    std::vector<Hand> hands;
    for (int i = 0; i < data.size(); ++i) {
        std::array<char, 5> cards; // NOLINT(*-pro-type-member-init)
        for (int c = 0; c < 5; ++c) {
            cards[c] = data[i];
            i++;
        }
        i++;
        int bid;
        i = parse_num(bid, data, i);
        hands.push_back({ cards, bid });
    }

    std::ranges::sort(hands, [](Hand& a, Hand& b) {
        if (a.type() != b.type()) {
            return a.type() < b.type();
        }
        for (int i = 0; i < 5; ++i) {
            const int a_idx = calc_card_index(a.cards[i]);
            // ReSharper disable once CppTooWideScopeInitStatement
            const int b_idx = calc_card_index(b.cards[i]);
            if (a_idx != b_idx) {
                return a_idx < b_idx;
            }
        }
        throw std::runtime_error("Hands are equal");
    });

    int total = 0;
    for (int i = 0; i < hands.size(); ++i) {
        total += hands[i].bid * (i + 1);
    }
    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-7-part-1/input.txt");

#ifdef BENCHMARK
    constexpr int n_runs = 10000;
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
