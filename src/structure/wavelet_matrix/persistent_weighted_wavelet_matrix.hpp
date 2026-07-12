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
struct PersistentWeightedWaveletMatrix :
    wavelet_matrix_detail::PersistentWeightedWaveletMatrixBase<
        T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>{
    using Base = wavelet_matrix_detail::PersistentWeightedWaveletMatrixBase<
        T, W, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>;
    using Base::Base;

    PersistentWeightedWaveletMatrix() = default;

    template<std::size_t N>
    PersistentWeightedWaveletMatrix(
        const std::array<T, N>& sequence,
        const std::array<W, N>& weights
    ): Base(
        std::vector<T>(sequence.begin(), sequence.end()),
        std::vector<W>(weights.begin(), weights.end())
    ){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int set(int version, int k, T value, W weight){
        return Base::set_from_version(version, k, value, weight);
    }
    int set_value(int version, int k, T value){
        return set(version, k, value, Base::weight(version, k));
    }
    int set_weight(int version, int k, W weight){
        return set(version, k, Base::access(version, k), weight);
    }
    int fork(int version){ return Base::fork_from_version(version); }
};
