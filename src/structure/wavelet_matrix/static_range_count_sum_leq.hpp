#pragma once

#include <limits>
#include <type_traits>
#include <vector>

#include "weighted_wavelet_matrix.hpp"

template<class W>
struct StaticRangeCountSumResult{
    int count;
    W sum;
};

template<
    class T,
    class W,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits
>
class StaticRangeCountSumLeq{
    static_assert(std::is_integral_v<T>);

    static std::vector<W> values_as_weights(const std::vector<T>& values){
        return std::vector<W>(values.begin(), values.end());
    }

    WeightedWaveletMatrix<T, W, MAX_SIZE, BIT_WIDTH> matrix_;

public:
    explicit StaticRangeCountSumLeq(const std::vector<T>& values):
        matrix_(values, values_as_weights(values)){}

    StaticRangeCountSumLeq(
        const std::vector<T>& values,
        const std::vector<W>& weights
    ): matrix_(values, weights){}

    StaticRangeCountSumResult<W> query(int left, int right, T upper) const{
        if(upper == std::numeric_limits<T>::max()){
            const W total = matrix_.sum(left, right);
            return {right - left, total};
        }
        const T exclusive_upper = static_cast<T>(upper + T{1});
        return {
            matrix_.range_freq(left, right, exclusive_upper),
            matrix_.range_sum(left, right, exclusive_upper),
        };
    }
};
