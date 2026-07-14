#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "../../structure/other/fenwick_tree.hpp"

template<class T, int MAX_SIZE = 100000>
long long count_decreasing_triplets(const std::vector<T>& values){
    std::vector<T> coordinates = values;
    std::sort(coordinates.begin(), coordinates.end());
    coordinates.erase(
        std::unique(coordinates.begin(), coordinates.end()),
        coordinates.end()
    );
    const int rank_count = static_cast<int>(coordinates.size());
    auto left = std::make_unique<FenwickTree<int, MAX_SIZE>>(rank_count);
    auto right = std::make_unique<FenwickTree<int, MAX_SIZE>>(rank_count);
    std::vector<int> rank;
    rank.reserve(values.size());
    for(const T& value: values){
        const int index = static_cast<int>(
            std::lower_bound(coordinates.begin(), coordinates.end(), value)
            - coordinates.begin()
        );
        rank.push_back(index);
        right->add(index, 1);
    }

    long long answer = 0;
    for(int position = 0; position < static_cast<int>(rank.size()); ++position){
        const int value_rank = rank[static_cast<std::size_t>(position)];
        right->add(value_rank, -1);
        const int greater_on_left = position - left->prefix_sum(value_rank + 1);
        const int smaller_on_right = right->prefix_sum(value_rank);
        answer += static_cast<long long>(greater_on_left) * smaller_on_right;
        left->add(value_rank, 1);
    }
    return answer;
}
