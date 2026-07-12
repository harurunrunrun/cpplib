#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <vector>
#include "compressed_weighted_wavelet_matrix.hpp"

template<
    class T,
    int MAX_SIZE,
    class Sum = long long,
    int ID_BIT_WIDTH = std::numeric_limits<unsigned>::digits
>
struct FunctionalWaveletMatrix :
    CompressedWeightedWaveletMatrix<T, Sum, MAX_SIZE, ID_BIT_WIDTH>{
    static_assert(std::is_constructible_v<Sum, T> || std::is_convertible_v<T, Sum>);
    using Base = CompressedWeightedWaveletMatrix<T, Sum, MAX_SIZE, ID_BIT_WIDTH>;

private:
    static std::vector<Sum> make_weights(const std::vector<T>& sequence){
        std::vector<Sum> weights(sequence.size());
        for(std::size_t k = 0; k < sequence.size(); k++){
            weights[k] = static_cast<Sum>(sequence[k]);
        }
        return weights;
    }

public:
    explicit FunctionalWaveletMatrix(const std::vector<T>& sequence):
        Base(sequence, make_weights(sequence)){}

    template<std::size_t N>
    explicit FunctionalWaveletMatrix(const std::array<T, N>& sequence):
        FunctionalWaveletMatrix(std::vector<T>(sequence.begin(), sequence.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }
};
