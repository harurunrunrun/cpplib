#pragma once

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <string>
#include <vector>

inline std::vector<std::size_t> longest_shared_prefix_lengths(
    const std::vector<std::string>& strings
){
    const std::size_t size = strings.size();
    std::vector<std::size_t> order(size);
    std::iota(order.begin(), order.end(), std::size_t{});
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        return strings[left] < strings[right];
    });

    auto lcp = [&](std::size_t left, std::size_t right){
        const std::size_t limit = std::min(strings[left].size(), strings[right].size());
        std::size_t length = 0;
        while(length < limit && strings[left][length] == strings[right][length]) ++length;
        return length;
    };

    std::vector<std::size_t> result(size);
    for(std::size_t index = 1; index < size; ++index){
        const std::size_t length = lcp(order[index - 1], order[index]);
        result[order[index - 1]] = std::max(result[order[index - 1]], length);
        result[order[index]] = std::max(result[order[index]], length);
    }
    return result;
}
