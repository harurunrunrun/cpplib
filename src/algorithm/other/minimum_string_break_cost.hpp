#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_string_break_cost(
    long long length,
    const std::vector<long long>& break_positions
){
    if(length <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: nonpositive length "
            "(minimum_string_break_cost)."
        );
    }
    if(!std::is_sorted(break_positions.begin(), break_positions.end()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: positions are not sorted "
            "(minimum_string_break_cost)."
        );
    }
    std::vector<long long> positions;
    positions.reserve(break_positions.size() + 2);
    positions.push_back(0);
    for(long long position: break_positions){
        if(position <= 0 || position >= length ||
           (!positions.empty() && positions.back() == position))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid break position "
                "(minimum_string_break_cost)."
            );
        }
        positions.push_back(position);
    }
    positions.push_back(length);
    const int size = static_cast<int>(positions.size());
    std::vector<std::vector<long long>> dp(
        static_cast<std::size_t>(size),
        std::vector<long long>(static_cast<std::size_t>(size))
    );
    std::vector<std::vector<int>> optimum(
        static_cast<std::size_t>(size),
        std::vector<int>(static_cast<std::size_t>(size))
    );
    for(int left = 0; left + 1 < size; left++){
        optimum[static_cast<std::size_t>(left)][static_cast<std::size_t>(left + 1)] = left + 1;
    }
    for(int span = 2; span < size; span++){
        for(int left = 0; left + span < size; left++){
            const int right = left + span;
            long long best = std::numeric_limits<long long>::max();
            int best_cut = -1;
            int begin = optimum[static_cast<std::size_t>(left)][static_cast<std::size_t>(right - 1)];
            int end = optimum[static_cast<std::size_t>(left + 1)][static_cast<std::size_t>(right)];
            begin = std::max(begin, left + 1);
            end = std::min(end, right - 1);
            for(int cut = begin; cut <= end; cut++){
                const long long candidate =
                    dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(cut)] +
                    dp[static_cast<std::size_t>(cut)][static_cast<std::size_t>(right)] +
                    positions[static_cast<std::size_t>(right)] -
                    positions[static_cast<std::size_t>(left)];
                if(candidate < best){
                    best = candidate;
                    best_cut = cut;
                }
            }
            dp[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)] = best;
            optimum[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)] = best_cut;
        }
    }
    return dp.front().back();
}
