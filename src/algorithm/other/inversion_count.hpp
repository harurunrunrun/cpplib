#ifndef CPPLIB_SRC_ALGORITHM_OTHER_INVERSION_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_INVERSION_COUNT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numeric>
#include <vector>

template<class T, class Compare = std::less<T>>
std::uint64_t inversion_count(
    const std::vector<T>& values,
    Compare compare = Compare{}
){
    const std::size_t n = values.size();
    std::vector<std::size_t> order(n);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::vector<std::size_t> buffer(n);
    std::uint64_t result = 0;

    for(std::size_t width = 1; width < n;){
        for(std::size_t left = 0; left < n;){
            const std::size_t middle = left + std::min(width, n - left);
            const std::size_t right = middle + std::min(width, n - middle);
            std::size_t left_index = left;
            std::size_t right_index = middle;
            std::size_t output = left;

            while(left_index < middle && right_index < right){
                if(compare(
                    values[order[right_index]],
                    values[order[left_index]]
                )){
                    buffer[output++] = order[right_index++];
                    result += static_cast<std::uint64_t>(middle - left_index);
                }else{
                    buffer[output++] = order[left_index++];
                }
            }
            while(left_index < middle){
                buffer[output++] = order[left_index++];
            }
            while(right_index < right){
                buffer[output++] = order[right_index++];
            }
            left = right;
        }
        order.swap(buffer);
        if(width > n / 2) break;
        width *= 2;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_INVERSION_COUNT_HPP_INCLUDED
