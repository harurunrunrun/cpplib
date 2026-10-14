#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIPLICATIVE_CONVOLUTION_POWER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIPLICATIVE_CONVOLUTION_POWER_HPP_INCLUDED

#include <bit>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
#include "multiplicative_convolution.hpp"

namespace multiplicative_power_internal{
template<class Mint, class Multiply>
std::vector<Mint> power(std::vector<Mint> values, std::uint64_t exponent,
                        const Multiply& multiply){
    std::vector<Mint> result(values.size());
    result[1 % values.size()] = 1;
    while(exponent){
        if(exponent & 1) result = multiply(result, values);
        exponent >>= 1;
        if(exponent) values = multiply(values, values);
    }
    return result;
}
} // namespace multiplicative_power_internal

// Weighted counts of ordered length-exponent sequences, classified by product.
template<int MOD=998244353>
std::vector<Modint<MOD>> prime_multiplicative_power(
    std::vector<Modint<MOD>> values, std::uint64_t exponent
){
    if(values.size() > static_cast<std::size_t>(0x7fffffff) ||
       !math::is_prime_miller_rabin(values.size()))
        throw std::invalid_argument("prime_multiplicative_power requires prime length");
    return multiplicative_power_internal::power(std::move(values), exponent,
        [](const auto& a,const auto& b){return prime_multiplicative_convolution(a,b);});
}

template<int MOD=998244353>
std::vector<Modint<MOD>> power_two_multiplicative_power(
    std::vector<Modint<MOD>> values, std::uint64_t exponent
){
    if(!std::has_single_bit(values.size()) || values.size() > (std::size_t(1)<<30))
        throw std::invalid_argument("power_two_multiplicative_power requires power-of-two length");
    return multiplicative_power_internal::power(std::move(values), exponent,
        [](const auto& a,const auto& b){return power_two_multiplicative_convolution(a,b);});
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_MULTIPLICATIVE_CONVOLUTION_POWER_HPP_INCLUDED
