#ifndef CPPLIB_SRC_ALGORITHM_RANGE_SLIDING_WINDOW_WINDOW_MEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_SLIDING_WINDOW_WINDOW_MEX_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../../../structure/ordered_set/integer/mex_multiset.hpp"

inline std::vector<std::size_t> sliding_window_mexes(
    const std::vector<std::int64_t>& values,
    std::size_t window
){
    if(window == 0 || window > values.size()){
        throw std::out_of_range("sliding_window_mexes: invalid window length");
    }
    const std::size_t universe = window + 1;
    const auto tracked = [universe](std::int64_t value){
        return value >= 0 && static_cast<std::uint64_t>(value) < universe;
    };
    MexMultiset multiset(universe);
    for(std::size_t index = 0; index < window; index++){
        if(tracked(values[index])) multiset.insert(static_cast<std::size_t>(values[index]));
    }
    std::vector<std::size_t> answer;
    answer.reserve(values.size() - window + 1);
    answer.push_back(multiset.mex());
    for(std::size_t right = window; right < values.size(); right++){
        const std::int64_t removed = values[right - window];
        if(tracked(removed)) multiset.erase(static_cast<std::size_t>(removed));
        const std::int64_t added = values[right];
        if(tracked(added)) multiset.insert(static_cast<std::size_t>(added));
        answer.push_back(multiset.mex());
    }
    return answer;
}

inline std::size_t minimum_fixed_window_mex(
    const std::vector<std::int64_t>& values,
    std::size_t window
){
    if(window == 0 || window > values.size()){
        throw std::out_of_range("minimum_fixed_window_mex: invalid window length");
    }
    const std::size_t universe = values.size() + 1;
    std::vector<std::int64_t> last(universe, -1);
    std::vector<bool> absent_from_a_window(universe, false);
    for(std::size_t index = 0; index < values.size(); index++){
        const std::int64_t value = values[index];
        if(value < 0 || static_cast<std::uint64_t>(value) >= universe) continue;
        const std::size_t converted = static_cast<std::size_t>(value);
        if(static_cast<std::int64_t>(index) - last[converted] > static_cast<std::int64_t>(window)){
            absent_from_a_window[converted] = true;
        }
        last[converted] = static_cast<std::int64_t>(index);
    }
    for(std::size_t value = 0; value < universe; value++){
        if(static_cast<std::int64_t>(values.size()) - last[value] > static_cast<std::int64_t>(window)){
            absent_from_a_window[value] = true;
        }
        if(absent_from_a_window[value]) return value;
    }
    return universe;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_SLIDING_WINDOW_WINDOW_MEX_HPP_INCLUDED
