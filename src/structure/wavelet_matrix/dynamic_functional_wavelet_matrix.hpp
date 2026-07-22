#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_FUNCTIONAL_WAVELET_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_FUNCTIONAL_WAVELET_MATRIX_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "dynamic_weighted_wavelet_matrix.hpp"

template<
    class T,
    int MAX_SIZE,
    class Sum = long long,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits
>
struct DynamicFunctionalWaveletMatrix :
    DynamicWeightedWaveletMatrix<T, Sum, MAX_SIZE, BIT_WIDTH>{
    static_assert(std::is_constructible_v<Sum, T> || std::is_convertible_v<T, Sum>);
    using Base = DynamicWeightedWaveletMatrix<T, Sum, MAX_SIZE, BIT_WIDTH>;

private:
    static std::vector<Sum> make_weights(const std::vector<T>& sequence){
        std::vector<Sum> weights(sequence.size());
        for(std::size_t i = 0; i < sequence.size(); i++){
            weights[i] = static_cast<Sum>(sequence[i]);
        }
        return weights;
    }

public:
    DynamicFunctionalWaveletMatrix() = default;

    explicit DynamicFunctionalWaveletMatrix(const std::vector<T>& sequence):
        Base(sequence, make_weights(sequence)){}

    template<std::size_t N>
    explicit DynamicFunctionalWaveletMatrix(const std::array<T, N>& sequence):
        DynamicFunctionalWaveletMatrix(std::vector<T>(sequence.begin(), sequence.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    void set(int k, T value){
        Base::set(k, value, static_cast<Sum>(value));
    }

    void set_value(int k, T value){ set(k, value); }

    void insert(int position, T value){
        Base::insert(position, value, static_cast<Sum>(value));
    }

    T erase(int position){ return Base::erase(position).first; }

    void push_back(T value){ insert(this->size(), value); }

    T pop_back(){
        if(this->size() == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (pop_back)."
            );
        }
        return erase(this->size() - 1);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_FUNCTIONAL_WAVELET_MATRIX_HPP_INCLUDED
