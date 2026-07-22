#ifndef CPPLIB_SRC_ALGORITHM_RANGE_COUNT_UNBLOCKED_HEIGHT_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_COUNT_UNBLOCKED_HEIGHT_QUERIES_HPP_INCLUDED

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../structure/other/sparse_table.hpp"
#include "../../structure/types/common_monoids.hpp"

template<int MAX_SIZE = 50000>
int count_unblocked_height_queries(
    const std::vector<int>& height,
    const std::vector<std::pair<int, int>>& queries
){
    constexpr MaxMonoid<int> maximum{};
    const int size = static_cast<int>(height.size());
    auto table = std::make_unique<SparseTable<maximum, MAX_SIZE>>(height);
    int answer = 0;
    for(const auto [from, to]: queries){
        if(from < 0 || to < 0 || size <= from || size <= to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range "
                "(count_unblocked_height_queries)."
            );
        }
        const int left = std::min(from, to);
        const int right = std::max(from, to);
        if(right - left <= 1
            || table->prod(left + 1, right) <= height[static_cast<std::size_t>(from)]){
            ++answer;
        }
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_COUNT_UNBLOCKED_HEIGHT_QUERIES_HPP_INCLUDED
