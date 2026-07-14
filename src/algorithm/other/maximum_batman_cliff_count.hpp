#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

inline int maximum_batman_cliff_count(
    const std::vector<int>& heights,
    int robin_index
){
    const int n = static_cast<int>(heights.size());
    if(robin_index < 0 || robin_index >= n)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid Robin index "
            "(maximum_batman_cliff_count)."
        );
    }
    std::vector<int> values = heights;
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    std::vector<int> fenwick(values.size() + 1);
    auto query = [&](int index){
        int result = 0;
        for(; index > 0; index -= index & -index){
            result = std::max(result, fenwick[static_cast<std::size_t>(index)]);
        }
        return result;
    };
    auto update = [&](int index, int value){
        for(; index < static_cast<int>(fenwick.size()); index += index & -index){
            fenwick[static_cast<std::size_t>(index)] =
                std::max(fenwick[static_cast<std::size_t>(index)], value);
        }
    };
    int answer = 0;
    int at_robin = 0;
    for(int index = 0; index < n; index++){
        const int ascending_rank = static_cast<int>(
            std::lower_bound(values.begin(), values.end(), heights[static_cast<std::size_t>(index)]) -
            values.begin()
        );
        const int reverse_rank = static_cast<int>(values.size()) - ascending_rank;
        int best = 1 + query(reverse_rank - 1);
        if(index > robin_index) best = std::max(best, at_robin + 1);
        if(index == robin_index) at_robin = best;
        update(reverse_rank, best);
        answer = std::max(answer, best);
    }
    return answer;
}
