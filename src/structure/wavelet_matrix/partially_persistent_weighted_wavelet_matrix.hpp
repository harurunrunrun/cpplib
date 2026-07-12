#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <vector>
#include "detail/persistent_weighted_wavelet_matrix_base.hpp"

template<
    class T,
    class W,
    int MAX_SIZE,
    int MAX_VERSION,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits,
    int BLOCK_SIZE = 512
>
struct PartiallyPersistentWeightedWaveletMatrix :
    wavelet_matrix_detail::PersistentWeightedWaveletMatrixBase<
        T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>{
    using Base = wavelet_matrix_detail::PersistentWeightedWaveletMatrixBase<
        T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>;
    using Base::Base;

    PartiallyPersistentWeightedWaveletMatrix() = default;

    template<std::size_t N>
    PartiallyPersistentWeightedWaveletMatrix(
        const std::array<T, N>& sequence,
        const std::array<W, N>& weights
    ): Base(
        std::vector<T>(sequence.begin(), sequence.end()),
        std::vector<W>(weights.begin(), weights.end())
    ){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int set(int k, T value, W weight){
        return Base::set_from_version(Base::latest_version(), k, value, weight);
    }
    int set_value(int k, T value){
        return set(k, value, Base::weight(Base::latest_version(), k));
    }
    int set_weight(int k, W weight){
        return set(k, Base::access(Base::latest_version(), k), weight);
    }
};
