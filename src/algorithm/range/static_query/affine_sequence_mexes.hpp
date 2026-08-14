#ifndef CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_AFFINE_SEQUENCE_MEXES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_AFFINE_SEQUENCE_MEXES_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace affine_sequence_mex_detail{
inline __int128 floor_div(__int128 numerator, __int128 denominator){
    __int128 quotient = numerator / denominator;
    const __int128 remainder = numerator % denominator;
    if(remainder != 0 && ((remainder < 0) != (denominator < 0))) --quotient;
    return quotient;
}
inline __int128 ceil_div(__int128 numerator, __int128 denominator){
    return -floor_div(-numerator, denominator);
}
}  // namespace affine_sequence_mex_detail

inline std::vector<std::size_t> affine_sequence_mexes(
    const std::vector<std::int64_t>& initial,
    const std::vector<std::int64_t>& slope,
    std::size_t step_count
){
    if(initial.size() != slope.size()){
        throw std::invalid_argument("affine_sequence_mexes: initial and slope sizes differ");
    }
    const std::size_t limit = initial.size();
    std::vector<std::vector<std::size_t>> values_at(step_count);
    std::vector<bool> constant_present(limit + 1, false);
    for(std::size_t index = 0; index < initial.size(); index++){
        const __int128 a = initial[index];
        const __int128 b = slope[index];
        if(b == 0){
            if(0 <= a && a <= static_cast<__int128>(limit)){
                constant_present[static_cast<std::size_t>(a)] = true;
            }
            continue;
        }
        __int128 low;
        __int128 high;
        if(b > 0){
            low = affine_sequence_mex_detail::ceil_div(-a, b);
            high = affine_sequence_mex_detail::floor_div(static_cast<__int128>(limit) - a, b);
        }else{
            low = affine_sequence_mex_detail::ceil_div(static_cast<__int128>(limit) - a, b);
            high = affine_sequence_mex_detail::floor_div(-a, b);
        }
        low = std::max<__int128>(low, 1);
        high = std::min<__int128>(high, static_cast<__int128>(step_count));
        for(__int128 step = low; step <= high; step++){
            const __int128 value = a + b * step;
            values_at[static_cast<std::size_t>(step - 1)].push_back(static_cast<std::size_t>(value));
        }
    }

    std::vector<std::size_t> answer(step_count, 0);
    std::vector<std::size_t> stamp(limit + 1, 0);
    for(std::size_t step = 0; step < step_count; step++){
        const std::size_t token = step + 1;
        for(const std::size_t value: values_at[step]) stamp[value] = token;
        while(answer[step] <= limit &&
              (constant_present[answer[step]] || stamp[answer[step]] == token)){
            ++answer[step];
        }
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_AFFINE_SEQUENCE_MEXES_HPP_INCLUDED
