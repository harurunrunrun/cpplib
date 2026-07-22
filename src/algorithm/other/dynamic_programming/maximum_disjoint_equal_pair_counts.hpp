#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_DISJOINT_EQUAL_PAIR_COUNTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_DISJOINT_EQUAL_PAIR_COUNTS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../offline_query/mo.hpp"

template<class T>
std::vector<int> maximum_disjoint_equal_pair_counts(
    const std::vector<T>& values,
    const std::vector<std::pair<int, int>>& ranges
){
    if(values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size overflow "
            "(maximum_disjoint_equal_pair_counts)."
        );
    }

    const int n = static_cast<int>(values.size());
    std::vector<T> order(values.begin(), values.end());
    std::sort(order.begin(), order.end());
    order.erase(std::unique(order.begin(), order.end()), order.end());

    std::vector<int> compressed(static_cast<std::size_t>(n));
    for(int i = 0; i < n; ++i){
        compressed[static_cast<std::size_t>(i)] = static_cast<int>(
            std::lower_bound(order.begin(), order.end(), values[static_cast<std::size_t>(i)])
            - order.begin()
        );
    }

    Mo mo(n);
    for(const auto& [left, right]: ranges) mo.add_query(left, right);

    std::vector<int> frequency(order.size(), 0);
    std::vector<int> result(ranges.size(), 0);
    int pair_count = 0;
    const auto add = [&](int index){
        int& count = frequency[static_cast<std::size_t>(compressed[static_cast<std::size_t>(index)])];
        pair_count -= count / 2;
        ++count;
        pair_count += count / 2;
    };
    const auto erase = [&](int index){
        int& count = frequency[static_cast<std::size_t>(compressed[static_cast<std::size_t>(index)])];
        pair_count -= count / 2;
        --count;
        pair_count += count / 2;
    };
    mo.solve(add, erase, [&](int query_index){
        result[static_cast<std::size_t>(query_index)] = pair_count;
    });
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MAXIMUM_DISJOINT_EQUAL_PAIR_COUNTS_HPP_INCLUDED
