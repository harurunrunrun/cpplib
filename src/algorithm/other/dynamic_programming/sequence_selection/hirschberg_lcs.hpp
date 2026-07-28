#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_SEQUENCE_SELECTION_HIRSCHBERG_LCS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_SEQUENCE_SELECTION_HIRSCHBERG_LCS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>

namespace hirschberg_lcs_internal{

template<class Value>
std::vector<int> prefix_lengths(
    std::span<const Value> first,
    std::span<const Value> second
){
    std::vector<int> previous(second.size() + 1, 0);
    std::vector<int> current(second.size() + 1, 0);
    for(const Value& value: first){
        for(std::size_t index = 0; index < second.size(); ++index){
            if(value == second[index]){
                current[index + 1] = previous[index] + 1;
            }else{
                current[index + 1] =
                    std::max(current[index], previous[index + 1]);
            }
        }
        std::swap(previous, current);
        std::fill(current.begin(), current.end(), 0);
    }
    return previous;
}

template<class Value>
std::vector<int> suffix_lengths(
    std::span<const Value> first,
    std::span<const Value> second
){
    std::vector<int> previous(second.size() + 1, 0);
    std::vector<int> current(second.size() + 1, 0);
    for(auto first_iterator = first.rbegin();
        first_iterator != first.rend();
        ++first_iterator){
        for(std::size_t index = second.size(); index-- > 0;){
            if(*first_iterator == second[index]){
                current[index] = previous[index + 1] + 1;
            }else{
                current[index] =
                    std::max(current[index + 1], previous[index]);
            }
        }
        std::swap(previous, current);
        std::fill(current.begin(), current.end(), 0);
    }
    return previous;
}

template<class Value>
void solve(
    std::span<const Value> first,
    std::span<const Value> second,
    std::vector<Value>& result
){
    if(first.empty() || second.empty()) return;
    if(first.size() == 1){
        if(std::find(second.begin(), second.end(), first.front())
            != second.end()){
            result.push_back(first.front());
        }
        return;
    }
    const std::size_t middle = first.size() / 2;
    const auto left = prefix_lengths(first.first(middle), second);
    const auto right = suffix_lengths(first.subspan(middle), second);
    std::size_t split = 0;
    for(std::size_t index = 1; index <= second.size(); ++index){
        if(left[index] + right[index] > left[split] + right[split]){
            split = index;
        }
    }
    solve(first.first(middle), second.first(split), result);
    solve(first.subspan(middle), second.subspan(split), result);
}

} // namespace hirschberg_lcs_internal

template<class Value>
std::vector<Value> hirschberg_lcs(
    const std::vector<Value>& first,
    const std::vector<Value>& second
){
    std::vector<Value> result;
    result.reserve(std::min(first.size(), second.size()));
    hirschberg_lcs_internal::solve<Value>(first, second, result);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_SEQUENCE_SELECTION_HIRSCHBERG_LCS_HPP_INCLUDED
