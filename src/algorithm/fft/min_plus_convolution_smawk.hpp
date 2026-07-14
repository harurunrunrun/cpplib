#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "min_plus_convolution.hpp"
#include "../other/smawk.hpp"

template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_arbitrary_smawk(
    const std::vector<T>& convex,
    const std::vector<T>& arbitrary
){
    static_assert(sizeof(T) <= sizeof(long long));
    if(convex.empty() || arbitrary.empty()) return {};
    if(!is_discrete_convex(convex))[[unlikely]]{
        throw std::invalid_argument(
            "min_plus_convolution_convex_arbitrary_smawk requires a convex first sequence"
        );
    }
    if(convex.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       arbitrary.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       convex.size() + arbitrary.size() - 1 >
           static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(
            "min-plus convolution input is too large for SMAWK indices"
        );
    }

    const auto [minimum_convex, maximum_convex] =
        std::minmax_element(convex.begin(), convex.end());
    const auto [minimum_arbitrary, maximum_arbitrary] =
        std::minmax_element(arbitrary.begin(), arbitrary.end());
    const __int128_t penalty =
        static_cast<__int128_t>(*maximum_convex) - *minimum_convex +
        static_cast<__int128_t>(*maximum_arbitrary) - *minimum_arbitrary + 1;
    const int convex_size = static_cast<int>(convex.size());
    const int arbitrary_size = static_cast<int>(arbitrary.size());
    const int output_size = convex_size + arbitrary_size - 1;

    const std::vector<int> minima = smawk_row_minima(
        output_size,
        arbitrary_size,
        [&](int row, int candidate){
            const int convex_index = row - candidate;
            __int128_t convex_value;
            if(convex_index < 0){
                convex_value = static_cast<__int128_t>(convex.front()) -
                    static_cast<__int128_t>(convex_index) * penalty;
            }else if(convex_size <= convex_index){
                convex_value = static_cast<__int128_t>(convex.back()) +
                    static_cast<__int128_t>(convex_index - convex_size + 1) *
                        penalty;
            }else{
                convex_value = convex[static_cast<std::size_t>(convex_index)];
            }
            return convex_value +
                static_cast<__int128_t>(arbitrary[static_cast<std::size_t>(candidate)]);
        }
    );

    std::vector<T> result(static_cast<std::size_t>(output_size));
    for(int index = 0; index < output_size; ++index){
        const int candidate = minima[static_cast<std::size_t>(index)];
        const int convex_index = index - candidate;
        if(convex_index < 0 || convex_size <= convex_index)[[unlikely]]{
            throw std::logic_error("SMAWK returned a padded min-plus candidate");
        }
        const __int128_t value =
            static_cast<__int128_t>(convex[static_cast<std::size_t>(convex_index)]) +
            arbitrary[static_cast<std::size_t>(candidate)];
        if(value < static_cast<__int128_t>(std::numeric_limits<T>::min()) ||
           static_cast<__int128_t>(std::numeric_limits<T>::max()) < value){
            throw std::overflow_error(
                "min-plus convolution result is outside the value type"
            );
        }
        result[static_cast<std::size_t>(index)] = static_cast<T>(value);
    }
    return result;
}
