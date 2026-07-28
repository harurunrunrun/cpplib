#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_CDQ_DIVIDE_AND_CONQUER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_CDQ_DIVIDE_AND_CONQUER_HPP_INCLUDED

#include <stdexcept>
#include <utility>

template<class Merge>
void cdq_divide_and_conquer(int left, int right, Merge&& merge){
    if(left < 0 || right < left) throw std::invalid_argument("cdq_divide_and_conquer: invalid range");
    if(right - left <= 1) return;
    const int middle = left + (right - left) / 2;
    cdq_divide_and_conquer(left, middle, merge);
    cdq_divide_and_conquer(middle, right, merge);
    merge(left, middle, right);
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_CDQ_DIVIDE_AND_CONQUER_HPP_INCLUDED
