#pragma once

#include "prefix_function.hpp"

template<class Sequence>
int whole_string_repetition_count(const Sequence& sequence){
    const int size = static_cast<int>(sequence.size());
    if(size == 0) return 0;
    const std::vector<int> failure = prefix_function(sequence);
    const int period = size - failure.back();
    return size % period == 0 ? size / period : 1;
}
