#ifndef CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_LONGEST_INCREASING_SUBSEQUENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_LONGEST_INCREASING_SUBSEQUENCE_HPP_INCLUDED

#include <algorithm>
#include <functional>
#include <vector>

template<class T, class Compare = std::less<T>>
std::vector<int> longest_increasing_subsequence(
    const std::vector<T>& values,
    bool strict = true,
    Compare compare = Compare()
){
    const int n = static_cast<int>(values.size());
    std::vector<T> tail_values;
    std::vector<int> tail_indices;
    std::vector<int> previous(static_cast<std::size_t>(n), -1);
    tail_values.reserve(values.size());
    tail_indices.reserve(values.size());

    for(int i = 0; i < n; i++){
        const T& value = values[static_cast<std::size_t>(i)];
        auto iterator = strict
            ? std::lower_bound(tail_values.begin(), tail_values.end(), value, compare)
            : std::upper_bound(tail_values.begin(), tail_values.end(), value, compare);
        const int length = static_cast<int>(iterator - tail_values.begin());
        if(length > 0){
            previous[static_cast<std::size_t>(i)] =
                tail_indices[static_cast<std::size_t>(length - 1)];
        }
        if(iterator == tail_values.end()){
            tail_values.push_back(value);
            tail_indices.push_back(i);
        }else{
            *iterator = value;
            tail_indices[static_cast<std::size_t>(length)] = i;
        }
    }

    std::vector<int> result(tail_values.size());
    if(result.empty()) return result;
    int current = tail_indices.back();
    for(int i = static_cast<int>(result.size()) - 1; i >= 0; i--){
        result[static_cast<std::size_t>(i)] = current;
        current = previous[static_cast<std::size_t>(current)];
    }
    return result;
}

template<class T, class Compare = std::less<T>>
int longest_increasing_subsequence_length(
    const std::vector<T>& values,
    bool strict = true,
    Compare compare = Compare()
){
    std::vector<T> tails;
    tails.reserve(values.size());
    for(const T& value: values){
        auto iterator = strict
            ? std::lower_bound(tails.begin(), tails.end(), value, compare)
            : std::upper_bound(tails.begin(), tails.end(), value, compare);
        if(iterator == tails.end()) tails.push_back(value);
        else *iterator = value;
    }
    return static_cast<int>(tails.size());
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_LONGEST_INCREASING_SUBSEQUENCE_HPP_INCLUDED
