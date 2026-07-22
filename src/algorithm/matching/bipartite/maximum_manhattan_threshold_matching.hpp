#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_MANHATTAN_THRESHOLD_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_MANHATTAN_THRESHOLD_MATCHING_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "maximum_bipartite_pair_count.hpp"

inline int maximum_manhattan_threshold_matching(
    const std::vector<std::pair<long long, long long>>& left_points,
    const std::vector<std::pair<long long, long long>>& right_points,
    long long maximum_distance
){
    if(maximum_distance < 0)[[unlikely]]{
        throw std::invalid_argument(
            "maximum_manhattan_threshold_matching negative threshold"
        );
    }
    if(left_points.size() >
           static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       right_points.size() >
           static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(
            "maximum_manhattan_threshold_matching too many points"
        );
    }

    const auto absolute = [](__int128_t value){
        return value < 0 ? -value : value;
    };
    const __int128_t limit = maximum_distance;
    std::vector<std::pair<int, int>> allowed_pairs;
    for(int left = 0; left < static_cast<int>(left_points.size()); ++left){
        const auto [left_x, left_y] =
            left_points[static_cast<std::size_t>(left)];
        for(int right = 0; right < static_cast<int>(right_points.size()); ++right){
            const auto [right_x, right_y] =
                right_points[static_cast<std::size_t>(right)];
            const __int128_t distance =
                absolute(static_cast<__int128_t>(left_x) - right_x) +
                absolute(static_cast<__int128_t>(left_y) - right_y);
            if(distance <= limit) allowed_pairs.emplace_back(left, right);
        }
    }
    return maximum_bipartite_pair_count(
        static_cast<int>(left_points.size()),
        static_cast<int>(right_points.size()),
        allowed_pairs
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_MANHATTAN_THRESHOLD_MATCHING_HPP_INCLUDED
