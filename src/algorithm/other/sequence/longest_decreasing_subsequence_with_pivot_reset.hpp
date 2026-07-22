#ifndef CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_LONGEST_DECREASING_SUBSEQUENCE_WITH_PIVOT_RESET_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_LONGEST_DECREASING_SUBSEQUENCE_WITH_PIVOT_RESET_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline int longest_decreasing_subsequence_with_pivot_reset(
    const std::vector<int>& values,
    int pivot_index
){
    if(values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(
            "library assertion fault: sequence is too long "
            "(longest_decreasing_subsequence_with_pivot_reset)."
        );
    }
    const int n = static_cast<int>(values.size());
    if(pivot_index < 0 || pivot_index >= n)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: pivot_index is out of range "
            "(longest_decreasing_subsequence_with_pivot_reset)."
        );
    }
    std::vector<int> coordinates = values;
    std::sort(coordinates.begin(), coordinates.end());
    coordinates.erase(std::unique(coordinates.begin(), coordinates.end()), coordinates.end());
    std::vector<int> fenwick(coordinates.size() + 1);
    auto query = [&](std::size_t index){
        int result = 0;
        for(; index > 0; index -= index & -index){
            result = std::max(result, fenwick[index]);
        }
        return result;
    };
    auto update = [&](std::size_t index, int value){
        for(; index < fenwick.size(); index += index & -index){
            fenwick[index] = std::max(fenwick[index], value);
        }
    };
    int answer = 0;
    int at_pivot = 0;
    for(int index = 0; index < n; index++){
        const std::size_t ascending_rank = static_cast<std::size_t>(
            std::lower_bound(coordinates.begin(), coordinates.end(), values[static_cast<std::size_t>(index)]) -
            coordinates.begin()
        );
        const std::size_t reverse_rank = coordinates.size() - ascending_rank;
        int best = 1 + query(reverse_rank - 1);
        if(index > pivot_index) best = std::max(best, at_pivot + 1);
        if(index == pivot_index) at_pivot = best;
        update(reverse_rank, best);
        answer = std::max(answer, best);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_LONGEST_DECREASING_SUBSEQUENCE_WITH_PIVOT_RESET_HPP_INCLUDED
