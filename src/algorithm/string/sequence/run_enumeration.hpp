#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_RUN_ENUMERATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_RUN_ENUMERATION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

#include "maximum_consecutive_repetitions.hpp"

struct StringRun{
    int left;
    int right;
    int period;

    friend bool operator==(const StringRun&, const StringRun&) = default;
};

namespace run_enumeration_internal{

inline void collect(
    const std::vector<int>& rank,
    const maximum_consecutive_repetitions_internal::LcpQuery& forward,
    const maximum_consecutive_repetitions_internal::LcpQuery& backward,
    std::vector<StringRun>& result
){
    const int length = static_cast<int>(rank.size());
    std::vector<int> next_smaller(static_cast<std::size_t>(length), length);
    std::vector<int> stack;
    for(int index = 0; index < length; ++index){
        while(!stack.empty()
            && rank[static_cast<std::size_t>(index)]
                < rank[static_cast<std::size_t>(stack.back())]){
            next_smaller[static_cast<std::size_t>(stack.back())] = index;
            stack.pop_back();
        }
        stack.push_back(index);
    }
    for(int left = 0; left < length; ++left){
        const int right = next_smaller[static_cast<std::size_t>(left)];
        if(right == length) continue;
        const int period = right - left;
        const int extend_right = forward.query(left, right);
        const int extend_left = left == 0 ? 0 : backward.query(
            length - left, length - right
        );
        if(extend_left + extend_right < period) continue;
        result.push_back({
            left - extend_left, right + extend_right, period
        });
    }
}

} // namespace run_enumeration_internal

inline std::vector<StringRun> enumerate_runs(const std::string& text){
    const int length = static_cast<int>(text.size());
    if(length < 2) return {};
    const maximum_consecutive_repetitions_internal::LcpQuery forward(text);
    const std::string reversed(text.rbegin(), text.rend());
    const maximum_consecutive_repetitions_internal::LcpQuery backward(reversed);
    std::vector<StringRun> result;
    std::vector<int> rank = forward.ranks();
    run_enumeration_internal::collect(rank, forward, backward, result);
    for(int& value: rank) value = length - 1 - value;
    run_enumeration_internal::collect(rank, forward, backward, result);
    std::sort(result.begin(), result.end(), [](const StringRun& left,
                                              const StringRun& right){
        return std::tuple(left.left, left.right, left.period)
            < std::tuple(right.left, right.right, right.period);
    });
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_RUN_ENUMERATION_HPP_INCLUDED
