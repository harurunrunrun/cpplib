#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

#include "../../structure/other/priority_sum_structure.hpp"

template<
    class T,
    class Sum = T,
    class Compare = std::less<T>,
    class Projection = PrioritySumDefaultProjection<T, Sum>
>
std::vector<Sum> sliding_window_k_smallest_sums(
    const std::vector<T>& values,
    std::size_t window_size,
    std::size_t k,
    const Compare& compare = Compare(),
    const Projection& project = Projection()
){
    if(window_size == 0 || values.size() < window_size || window_size < k)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(sliding_window_k_smallest_sums)."
        );
    }

    std::vector<T> window(values.begin(), values.begin() + window_size);
    PrioritySumStructure<T, Sum, Compare, Projection> structure(
        window,
        k,
        compare,
        project
    );
    std::vector<Sum> result;
    result.reserve(values.size() - window_size + 1);
    result.push_back(structure.smallest_sum());
    for(std::size_t right = window_size; right < values.size(); ++right){
        structure.insert(values[right]);
        structure.erase(values[right - window_size]);
        result.push_back(structure.smallest_sum());
    }
    return result;
}
