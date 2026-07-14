#pragma once

#include <deque>
#include <functional>
#include <stdexcept>
#include <vector>

template<class T>
std::vector<T> sliding_window_maximum(
    const std::vector<T>& values,
    int window
){
    if(window <= 0 || static_cast<int>(values.size()) < window)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (sliding_window_maximum)."
        );
    }

    std::less<T> less;
    std::deque<int> candidates;
    std::vector<T> result;
    result.reserve(values.size() - static_cast<std::size_t>(window) + 1);
    for(int index = 0; index < static_cast<int>(values.size()); index++){
        while(!candidates.empty() && candidates.front() <= index - window){
            candidates.pop_front();
        }
        while(!candidates.empty() &&
              !less(values[static_cast<std::size_t>(index)],
                    values[static_cast<std::size_t>(candidates.back())])){
            candidates.pop_back();
        }
        candidates.push_back(index);
        if(index + 1 >= window){
            result.push_back(values[static_cast<std::size_t>(candidates.front())]);
        }
    }
    return result;
}
