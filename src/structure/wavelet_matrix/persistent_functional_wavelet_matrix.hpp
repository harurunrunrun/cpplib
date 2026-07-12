#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <vector>
#include "persistent_weighted_wavelet_matrix.hpp"

template<
    class T,
    int MAX_SIZE,
    int MAX_VERSION,
    class Sum = long long,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits,
    int BLOCK_SIZE = 512
>
struct PersistentFunctionalWaveletMatrix :
    PersistentWeightedWaveletMatrix<T, Sum, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>{
    static_assert(std::is_constructible_v<Sum, T> || std::is_convertible_v<T, Sum>);
    using Base = PersistentWeightedWaveletMatrix<T, Sum, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>;

private:
    static std::vector<Sum> make_weights(const std::vector<T>& sequence){
        std::vector<Sum> weights(sequence.size());
        for(std::size_t i = 0; i < sequence.size(); i++) weights[i] = static_cast<Sum>(sequence[i]);
        return weights;
    }

public:
    PersistentFunctionalWaveletMatrix() = default;
    explicit PersistentFunctionalWaveletMatrix(const std::vector<T>& sequence):
        Base(sequence, make_weights(sequence)){}

    template<std::size_t N>
    explicit PersistentFunctionalWaveletMatrix(const std::array<T, N>& sequence):
        PersistentFunctionalWaveletMatrix(std::vector<T>(sequence.begin(), sequence.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int set(int version, int k, T value){
        return Base::set(version, k, value, static_cast<Sum>(value));
    }
};
