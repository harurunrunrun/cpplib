#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline int minimum_exact_apple_cost(
    int maximum_packet_count,
    const std::vector<int>& packet_prices
){
    const int target_weight = static_cast<int>(packet_prices.size());
    if(maximum_packet_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative packet limit "
            "(minimum_exact_apple_cost)."
        );
    }
    for(int price: packet_prices){
        if(price == 0 || price < -1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid packet price "
                "(minimum_exact_apple_cost)."
            );
        }
    }
    constexpr int infinity = std::numeric_limits<int>::max() / 4;
    std::vector<int> previous(static_cast<std::size_t>(target_weight + 1), infinity);
    std::vector<int> current(static_cast<std::size_t>(target_weight + 1), infinity);
    previous[0] = 0;
    int answer = target_weight == 0 ? 0 : infinity;
    for(int count = 1; count <= maximum_packet_count; count++){
        std::fill(current.begin(), current.end(), infinity);
        for(int total = 1; total <= target_weight; total++){
            for(int weight = 1; weight <= total; weight++){
                const int price = packet_prices[static_cast<std::size_t>(weight - 1)];
                if(price < 0 || previous[static_cast<std::size_t>(total - weight)] == infinity) continue;
                current[static_cast<std::size_t>(total)] = std::min(
                    current[static_cast<std::size_t>(total)],
                    previous[static_cast<std::size_t>(total - weight)] + price
                );
            }
        }
        answer = std::min(answer, current[static_cast<std::size_t>(target_weight)]);
        previous.swap(current);
    }
    return answer == infinity ? -1 : answer;
}
