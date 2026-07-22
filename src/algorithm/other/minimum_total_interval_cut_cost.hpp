#ifndef CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_TOTAL_INTERVAL_CUT_COST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_TOTAL_INTERVAL_CUT_COST_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_total_interval_cut_cost(
    long long interval_length,
    const std::vector<long long>& cut_positions
){
    if(interval_length <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: nonpositive interval_length "
            "(minimum_total_interval_cut_cost)."
        );
    }
    if(!std::is_sorted(cut_positions.begin(), cut_positions.end()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: positions are not sorted "
            "(minimum_total_interval_cut_cost)."
        );
    }
    std::vector<long long> positions;
    positions.reserve(cut_positions.size() + 2);
    positions.push_back(0);
    for(long long position: cut_positions){
        if(position <= 0 || position >= interval_length ||
           (!positions.empty() && positions.back() == position))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid cut position "
                "(minimum_total_interval_cut_cost)."
            );
        }
        positions.push_back(position);
    }
    positions.push_back(interval_length);
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

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_MINIMUM_TOTAL_INTERVAL_CUT_COST_HPP_INCLUDED
