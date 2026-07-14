#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

inline int maximum_non_crossing_bridges(
    const std::vector<int>& first_side,
    const std::vector<int>& second_side
){
    if(first_side.size() != second_side.size())[[unlikely]]{
        throw std::invalid_argument("bridge endpoint count mismatch");
    }
    std::vector<std::pair<int, int>> bridges;
    if(first_side.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("bridge endpoint count is too large");
    }
    bridges.reserve(first_side.size());
    for(std::size_t index = 0; index < first_side.size(); ++index){
        bridges.emplace_back(first_side[index], second_side[index]);
    }
    std::sort(bridges.begin(), bridges.end());
    std::vector<int> tails;
    tails.reserve(bridges.size());
    for(const auto [unused, endpoint]: bridges){
        (void)unused;
        const auto iterator = std::upper_bound(
            tails.begin(), tails.end(), endpoint
        );
        if(iterator == tails.end()){
            tails.push_back(endpoint);
        }else{
            *iterator = endpoint;
        }
    }
    return static_cast<int>(tails.size());
}
