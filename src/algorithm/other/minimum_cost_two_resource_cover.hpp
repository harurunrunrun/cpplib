#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_COST_TWO_RESOURCE_COVER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_COST_TWO_RESOURCE_COVER_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

struct TwoResourceItem {
    int first_resource;
    int second_resource;
    long long cost;
};

inline long long minimum_cost_two_resource_cover(
    int required_first,
    int required_second,
    const std::vector<TwoResourceItem>& items
){
    if(required_first < 0 || required_second < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative resource requirement "
            "(minimum_cost_two_resource_cover)."
        );
    }
    const std::size_t height = static_cast<std::size_t>(required_first) + 1;
    const std::size_t width = static_cast<std::size_t>(required_second) + 1;
    if(height > std::numeric_limits<std::size_t>::max() / width)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: state space is too large "
            "(minimum_cost_two_resource_cover)."
        );
    }
    constexpr __int128 infinity = static_cast<__int128>(1) << 126;
    std::vector<__int128> dp(height * width, infinity);
    dp[0] = 0;
    for(const auto& item: items){
        if(item.first_resource < 0 || item.second_resource < 0 ||
           item.cost < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative item value "
                "(minimum_cost_two_resource_cover)."
            );
        }
        auto next = dp;
        for(int first = 0; first <= required_first; first++){
            for(int second = 0; second <= required_second; second++){
                const std::size_t source =
                    static_cast<std::size_t>(first) * width +
                    static_cast<std::size_t>(second);
                const __int128 current = dp[source];
                if(current == infinity) continue;
                const int next_first = static_cast<int>(std::min<long long>(
                    required_first,
                    static_cast<long long>(first) + item.first_resource
                ));
                const int next_second = static_cast<int>(std::min<long long>(
                    required_second,
                    static_cast<long long>(second) + item.second_resource
                ));
                auto& destination = next[
                    static_cast<std::size_t>(next_first) * width +
                    static_cast<std::size_t>(next_second)
                ];
                destination = std::min(destination, current + item.cost);
            }
        }
        dp.swap(next);
    }
    const __int128 answer = dp.back();
    if(answer == infinity) return -1;
    if(answer > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: result overflow "
            "(minimum_cost_two_resource_cover)."
        );
    }
    return static_cast<long long>(answer);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_COST_TWO_RESOURCE_COVER_HPP_INCLUDED
