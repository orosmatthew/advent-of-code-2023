#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
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

class Hand {
public:
    Hand(const std::array<char, 5> cards, const int bid)
        : m_cards(cards)
        , m_bid(bid)
        , m_hand_type(calc_type(cards))
        , m_tie_breaker(calc_tie_breaker(cards))
    {
    }

    [[nodiscard]] HandType type() const
    {
        return m_hand_type;
    }

    [[nodiscard]] int bid() const
    {
        return m_bid;
    }

    [[nodiscard]] const std::array<char, 5>& cards() const
    {
        return m_cards;
    }

    [[nodiscard]] int tie_breaker() const
    {
        return m_tie_breaker;
    }

private:
    static HandType calc_type(const std::array<char, 5>& cards)
    {
        std::array<int, 13> occurrences {};
        for (int i = 0; i < 5; ++i) {
            occurrences[calc_card_index(cards[i])]++;
        }

        auto calc_basic_type = [&occurrences]() -> HandType {
            bool three = false;
            int pairs = 0;
            for (int i = 0; i < 13; ++i) {
                if (i == 9) {
                    continue;
                }
                if (occurrences[i] == 5) {
                    return HandType::five_of_kind;
                }
                if (occurrences[i] == 4) {
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
                    return HandType::full_house;
                }
                return HandType::three_of_kind;
            }
            if (pairs == 2) {
                return HandType::two_pair;
            }
            if (pairs == 1) {
                return HandType::one_pair;
            }
            return HandType::high_card;
        };

        const int jokers = occurrences[9];
        switch (calc_basic_type()) {
        case HandType::five_of_kind:
            return HandType::five_of_kind;
        case HandType::four_of_kind:
            if (jokers == 1) {
                return HandType::five_of_kind;
            }
            return HandType::four_of_kind;
        case HandType::full_house:
            if (jokers == 3 || jokers == 2) {
                return HandType::five_of_kind;
            }
            return HandType::full_house;
        case HandType::three_of_kind:
            if (jokers == 3 || jokers == 1) {
                return HandType::four_of_kind;
            }
            if (jokers == 2) {
                return HandType::five_of_kind;
            }
            return HandType::three_of_kind;
        case HandType::two_pair:
            if (jokers == 1) {
                return HandType::full_house;
            }
            return HandType::two_pair;
        case HandType::one_pair:
            if (jokers == 3) {
                return HandType::five_of_kind;
            }
            if (jokers == 2) {
                return HandType::four_of_kind;
            }
            if (jokers == 1) {
                return HandType::three_of_kind;
            }
            return HandType::one_pair;
        case HandType::high_card:
            if (jokers == 5) {
                return HandType::five_of_kind;
            }
            if (jokers == 4) {
                return HandType::five_of_kind;
            }
            if (jokers == 3) {
                return HandType::four_of_kind;
            }
            if (jokers == 2) {
                return HandType::three_of_kind;
            }
            if (jokers == 1) {
                return HandType::one_pair;
            }
            return HandType::high_card;
        }
        throw std::runtime_error("Unreachable");
    }

    static int calc_tie_breaker(const std::array<char, 5>& cards)
    {
        constexpr std::array adjusted { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 10, 11, 12 };
        int total = adjusted[calc_card_index(cards[0])] * 28561; // 13^4
        total += adjusted[calc_card_index(cards[1])] * 2197; // 13^3
        total += adjusted[calc_card_index(cards[2])] * 169; // 13^2
        total += adjusted[calc_card_index(cards[3])] * 13; // 13^1
        total += adjusted[calc_card_index(cards[4])];
        return total;
    }

    std::array<char, 5> m_cards;
    int m_bid;
    HandType m_hand_type;
    int m_tie_breaker;
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

        hands.emplace_back(cards, bid);
    }

    std::ranges::sort(hands, [](const Hand& a, const Hand& b) {
        if (a.type() != b.type()) {
            return a.type() < b.type();
        }
        return a.tie_breaker() < b.tie_breaker();
        throw std::runtime_error("Hands are equal");
    });

    int total = 0;
    for (int i = 0; i < hands.size(); ++i) {
        total += hands[i].bid() * (i + 1);
    }
    return total;
}

// #define BENCHMARK

int main()
{
    const std::string data = read_data("./day-7-part-2/input.txt");

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
