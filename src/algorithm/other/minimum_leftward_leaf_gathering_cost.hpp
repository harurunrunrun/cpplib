#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_leftward_leaf_gathering_cost(
    const std::vector<int>& weights,
    int pile_count
){
    const int n = static_cast<int>(weights.size());
    if(pile_count <= 0 || pile_count > n)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid pile count "
            "(minimum_leftward_leaf_gathering_cost)."
        );
    }
    std::vector<long long> prefix_weight(static_cast<std::size_t>(n + 1));
    std::vector<long long> prefix_moment(static_cast<std::size_t>(n + 1));
    for(int i = 1; i <= n; i++){
        if(weights[static_cast<std::size_t>(i - 1)] <= 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: nonpositive weight "
                "(minimum_leftward_leaf_gathering_cost)."
            );
        }
        prefix_weight[static_cast<std::size_t>(i)] =
            prefix_weight[static_cast<std::size_t>(i - 1)] +
            weights[static_cast<std::size_t>(i - 1)];
        prefix_moment[static_cast<std::size_t>(i)] =
            prefix_moment[static_cast<std::size_t>(i - 1)] +
            static_cast<long long>(i) * weights[static_cast<std::size_t>(i - 1)];
    }
    constexpr long long infinity = std::numeric_limits<long long>::max() / 4;
    std::vector<long long> previous(static_cast<std::size_t>(n + 1), infinity);
    std::vector<long long> current(static_cast<std::size_t>(n + 1), infinity);
    previous[0] = 0;
    struct Line {
        long long slope;
        long long intercept;
        __int128 evaluate(long long x) const {
            return static_cast<__int128>(slope) * x + intercept;
        }
    };
    for(int group = 1; group <= pile_count; group++){
        std::fill(current.begin(), current.end(), infinity);
        std::vector<Line> hull;
        hull.reserve(static_cast<std::size_t>(n - group + 1));
        std::size_t first = 0;
        auto append = [&](int split){
            const long long coefficient = split + 1LL;
            const Line next{
                -coefficient,
                previous[static_cast<std::size_t>(split)] -
                    prefix_moment[static_cast<std::size_t>(split)] +
                    coefficient * prefix_weight[static_cast<std::size_t>(split)]
            };
            while(hull.size() >= first + 2){
                const auto& a = hull[hull.size() - 2];
                const auto& b = hull.back();
                if(static_cast<__int128>(b.intercept - a.intercept) *
                       (b.slope - next.slope) <
                   static_cast<__int128>(next.intercept - b.intercept) *
                       (a.slope - b.slope)) break;
                hull.pop_back();
            }
            hull.push_back(next);
        };
        append(group - 1);
        for(int end = group; end <= n; end++){
            const long long x = prefix_weight[static_cast<std::size_t>(end)];
            while(first + 1 < hull.size() &&
                  hull[first + 1].evaluate(x) <= hull[first].evaluate(x)) first++;
            const __int128 value = prefix_moment[static_cast<std::size_t>(end)] +
                                   hull[first].evaluate(x);
            current[static_cast<std::size_t>(end)] = static_cast<long long>(value);
            if(end < n) append(end);
        }
        previous.swap(current);
    }
    return previous[static_cast<std::size_t>(n)];
}
