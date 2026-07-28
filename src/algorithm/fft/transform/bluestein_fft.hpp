#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_BLUESTEIN_FFT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_BLUESTEIN_FFT_HPP_INCLUDED

#include <cmath>
#include <complex>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "fft.hpp"

inline void bluestein_fourier_transform(
    std::vector<std::complex<double>>& values,
    const bool inverse = false
){
    const std::size_t size = values.size();
    if(size == 0) return;
    if(fft_internal::is_power_of_two(size)){
        fast_fourier_transform(values, inverse);
        return;
    }
    if(size > static_cast<std::size_t>(
        std::numeric_limits<int>::max() / 2 + 1
    ))[[unlikely]]{
        throw std::length_error("Bluestein transform size is too large");
    }
    std::size_t transform_size = 1;
    const std::size_t required = size * 2 - 1;
    while(transform_size < required) transform_size *= 2;

    std::vector<std::complex<double>> left(transform_size);
    std::vector<std::complex<double>> right(transform_size);
    const long double sign = inverse ? -1.0L : 1.0L;
    const long double pi = std::acos(-1.0L);
    for(std::size_t index = 0; index < size; ++index){
        const long double coordinate = static_cast<long double>(index);
        const long double angle = sign * pi * coordinate * coordinate
            / static_cast<long double>(size);
        const std::complex<double> chirp(
            static_cast<double>(std::cos(angle)),
            static_cast<double>(std::sin(angle))
        );
        const std::complex<double> inverse_chirp = std::conj(chirp);
        left[index] = values[index] * chirp;
        right[index] = inverse_chirp;
        if(index != 0) right[transform_size - index] = inverse_chirp;
    }
    fast_fourier_transform(left);
    fast_fourier_transform(right);
    for(std::size_t index = 0; index < transform_size; ++index){
        left[index] *= right[index];
    }
    fast_fourier_transform(left, true);
    for(std::size_t index = 0; index < size; ++index){
        const long double coordinate = static_cast<long double>(index);
        const long double angle = sign * pi * coordinate * coordinate
            / static_cast<long double>(size);
        const std::complex<double> chirp(
            static_cast<double>(std::cos(angle)),
            static_cast<double>(std::sin(angle))
        );
        values[index] = left[index] * chirp;
        if(inverse) values[index] /= static_cast<double>(size);
    }
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_BLUESTEIN_FFT_HPP_INCLUDED
